//
// Created by devyk on 2023/10/22.
//
#include "core/include/video_codec_backend_interface.h"
#include "video_codec_plugin_api.h"
#include "kkrtc_plugin_loader.h"
#include "video_packet.h"

namespace kkrtc {
    namespace codec_core {
        static std::mutex &getInitializationMutex() {
            static std::mutex initializationMutex;
            return initializationMutex;
        }

        class VideoCodecPlusinBackend : public IVideoCodecBackend {
        protected:
            void initVideoCodecAPI() {
                const char *init_name = "kkrtc_video_codec_plugin_init";
                FN_kkrtc_video_codec_plugin_init_t fn_init = reinterpret_cast<FN_kkrtc_video_codec_plugin_init_t>(dynamicLib_->getSymbol(
                        init_name));
                if (fn_init) {
                    KKLogInfoTag("VideoCodecPlusinBackend") << "Found entry: '" << init_name << "'";
                    for (int supported_api_version = VCODEC_API_VERSION;
                         supported_api_version >= 0; supported_api_version--) {
                        kkrtc_vcodec_api = fn_init(VCODEC_ABI_VERSION, supported_api_version, NULL);
                        if (kkrtc_vcodec_api)
                            break;
                        KKLogInfoTag("VideoCodecPlusinBackend")
                            << "Video Codec: plugins is incompatible (can't be initialized): "
                            << dynamicLib_->getName();
                        return;
                    }
                } else {
                    KKLogInfoTag("VideoCodecPlusinBackend") << "Video Codec: missing plugins init function: '"
                                                            << init_name
                                                            << "', file: "
                                                            << dynamicLib_->getName();
                }
            }

        public:
            std::shared_ptr<kkrtc::plugin::DynamicLib> dynamicLib_;
            const KkVideoCodecAPI *kkrtc_vcodec_api;
        public:
            VideoCodecPlusinBackend(const KKPtr<kkrtc::plugin::DynamicLib> &lib)
                    : dynamicLib_(lib), kkrtc_vcodec_api(nullptr) {
                initVideoCodecAPI();
            }

            KKPtr<IVideoCodec> CreateVideoEncodec(const KKMediaFormat &params,
                                                  kkrtc::KKLogObserver *logObserver,
                                                  VideoEncodedObserver *videoEncodedObserver,
                                                  kkrtc::PluginGlobParam *plugin_glob_params) const override;

            KKPtr<IVideoCodec> CreateVideoDecodec(const KKMediaFormat &params, kkrtc::KKLogObserver *logObserver,
                                                  VideoDecodedObserver *videoEncodedObserver,
                                                  kkrtc::PluginGlobParam *plugin_glob_params) const override;
        };

        class VideoCodecPlusinBackendFactory : public IVideoCodecBackendFactory {
        public:
            KKVideoCodecAPIs id_;
            const char *baseName_;
            KKPtr<VideoCodecPlusinBackend> backend_;
            bool initialized_;
        public:
            VideoCodecPlusinBackendFactory(KKVideoCodecAPIs id, const char *baseName) :
                    id_(id), baseName_(baseName),
                    initialized_(false) {
                // nothing, plugins are loaded on demand
            }

            KKPtr<IVideoCodecBackend> getBackend() const override {
                initBackend();
                return std::static_pointer_cast<IVideoCodecBackend>(backend_);
            }

            bool isBuiltIn() const override {
                return false;
            }

        protected:
            inline void initBackend() const {
                if (!initialized_) {
                    const_cast<VideoCodecPlusinBackendFactory *>(this)->initBackend_();
                }
            }

            void initBackend_() {
                std::lock_guard<std::mutex> lock(getInitializationMutex());
                try {
                    if (!initialized_)
                        loadPlugin();
                }
                catch (...) {
                    KKLogErrorTag("VideoCodecPluginBackendFactory")
                        << "Video Codeccessor: exception during plugins loading: "
                        << baseName_ << ". SKIP";
                }
                initialized_ = true;
            }

            void loadPlugin();
        };

        void VideoCodecPlusinBackendFactory::loadPlugin() {
            for (const plugin::FileSystemPath_t &plugin: kkrtc::plugin::getPluginCandidates(baseName_)) {
                auto lib = makePtr<kkrtc::plugin::DynamicLib>(plugin);
                if (!lib->isLoaded())
                    continue;
                try {
                    KKPtr<VideoCodecPlusinBackend> pluginBackend = makePtr<VideoCodecPlusinBackend>(lib);
                    if (!pluginBackend)
                        return;
                    if (pluginBackend->kkrtc_vcodec_api == NULL
                            ) {
                        KKLogErrorTag("VideoCodecPlusinBackend") << "pluginBackend->kkrtc_cap_api null";
                        return;
                    }
                    backend_ = pluginBackend;
                    return;
                }
                catch (...) {
                    KKLogWarnTag("VideoCodecPlusinBackend")
                        << "Video Codeccessor: exception during plugins initialization: "
                        << plugin::toPrintablePath(plugin) << ". SKIP";
                }
            }
        }

        class VideoCodecPlugin : public kkrtc::codec_core::IVideoCodec {
            const KkVideoCodecPluginAPI *video_codec_plugin_api;
            KKPtr<kkrtc::PluginGlobParam_t *> plugin_glob_param_;
            KkPluginVideoCodec kk_video_codec_;
        public:
            ~VideoCodecPlugin() {
                if (video_codec_plugin_api->flush != nullptr &&
                    0 != video_codec_plugin_api->flush(kk_video_codec_)) {
                    KKLogErrorTag("VideoCodecPlugin") << "Video Codec: Can't flush video_codec by plugins '";
                };
                if (video_codec_plugin_api->release != nullptr &&
                    0 != video_codec_plugin_api->release(kk_video_codec_)) {
                    KKLogErrorTag("VideoCodecPlugin") << "Video Codec: Can't release video_codec by plugins '";
                };
                kk_video_codec_ = nullptr;
                if (!plugin_glob_param_.empty()) {
                    plugin_glob_param_.release();
                }
            }

            void Initialize(const kkrtc::KKMediaFormat &mediaFormats) override {

            };

            /**
             * 编码
             */
            int SendFrame(const kkrtc::VideoFrame &videoframe) {
                if (video_codec_plugin_api->send_frame != nullptr &&
                    video_codec_plugin_api->send_frame(kk_video_codec_, (void *) &videoframe) == 0)
                    return 0;
                return -1;
            };

            /**
             * 解码
             * @return
             */
            int SendPacket(const kkrtc::VideoPacket &videoPacket) {
                if (video_codec_plugin_api->send_packet != nullptr &&
                    video_codec_plugin_api->send_frame(kk_video_codec_, (void *) &videoPacket) == 0)
                    return 0;
                return -1;
            };

            const KKMediaFormat &GetMediaFormats() const override {
                return kkrtc::KKMediaFormat();
            }

            int GetDomain() const override {
                return video_codec_plugin_api->vcodec_id;
            }


            bool IsStarted() const override {
                return kk_video_codec_ != nullptr;
            }


            VideoCodecPlugin(const KkVideoCodecPluginAPI *vcodec_plugin_api, KkPluginVideoCodec kk_vcodec)
                    : video_codec_plugin_api(vcodec_plugin_api), kk_video_codec_(kk_vcodec) {
                assert(video_codec_plugin_api);
                assert(kk_video_codec_);
            }

            static KKPtr<VideoCodecPlugin> create(const KkVideoCodecPluginAPI *vcodec_plugin_api,
                                                  const KKMediaFormat &mediaFormat,
                                                  kkrtc::KKLogObserver *kkLogObserver,
                                                  void *kkVideoDataObserver,
                                                  PluginGlobParam *plugin_glob_params) {
                assert(vcodec_plugin_api);
                KkPluginVideoCodec kk_vcodec = nullptr;

                std::vector<int> vint_params = mediaFormat.getIntVector();
                int *c_params = vint_params.data();
                unsigned n_params = (unsigned) (vint_params.size() / 2);
                plugin_glob_params->c_params = c_params;
                plugin_glob_params->n_params = n_params;
                if (vcodec_plugin_api->initialize(&kk_vcodec, (void *) plugin_glob_params) != 0) {
                    KKLogErrorTag("video-pro") << " vcodec_initialize error.";
                    return nullptr;
                }
                if (vcodec_plugin_api->set_log_callback(kk_vcodec, kkLogObserver) != 0) {
                    KKLogErrorTag("video-pro") << " set_log_callback error.";
                }

                if (vcodec_plugin_api->set_video_callback(kk_vcodec, kkVideoDataObserver) != 0) {
                    KKLogErrorTag("video-pro") << " vcodec_initialize error.";
                    return nullptr;
                }
                assert(kk_vcodec);
                return makePtr<VideoCodecPlugin>(vcodec_plugin_api, kk_vcodec);;
            }
        };


        KKPtr<IVideoCodec> VideoCodecPlusinBackend::CreateVideoEncodec(const KKMediaFormat &params,
                                                                       kkrtc::KKLogObserver *logObserver,
                                                                       VideoEncodedObserver *videoEncodedObserver,
                                                                       kkrtc::PluginGlobParam *plugin_glob_params) const {
            try {
                if (kkrtc_vcodec_api) {
                    return VideoCodecPlugin::create(kkrtc_vcodec_api, params, logObserver, videoEncodedObserver,
                                                    plugin_glob_params);
                }
            }
            catch (...) {
                KKLogErrorTag("kkrtc_vcodec_backend_plugin") << "Video Codeccessor: can't create Video Codeccessor ";
                throw;
            }
            return nullptr;
        }

        KKPtr<IVideoCodec> VideoCodecPlusinBackend::CreateVideoDecodec(const KKMediaFormat &params,
                                                                       kkrtc::KKLogObserver *logObserver,
                                                                       VideoDecodedObserver *videoDecodedObserver,
                                                                       kkrtc::PluginGlobParam *plugin_glob_params) const {
            try {
                if (kkrtc_vcodec_api) {
                    return VideoCodecPlugin::create(kkrtc_vcodec_api, params, logObserver, videoDecodedObserver,
                                                    plugin_glob_params);
                }
            }
            catch (...) {
                KKLogErrorTag("kkrtc_vcodec_backend_plugin") << "Video Codeccessor: can't create Video Codeccessor ";
                throw;
            }
            return nullptr;
        }

        KKPtr<IVideoCodecBackendFactory>
        createVideoCodecPluginBackendFactory(KKVideoCodecAPIs id, const char *baseName) {
#if KKRTC_HAVE_FILESYSTEM_SUPPORT && defined(ENABLE_PLUGINS)
            return makePtr<VideoCodecPlusinBackendFactory>(id, baseName); //.staticCast<IBackendFactory>();
#else
            void(id);
            void(baseName);
            return KKPtr<IVideoCodecBackendFactory>();
#endif
        };


    }//codec_core
}//kkrtc