//
// Created by devyk on 2023/10/21.
//
#include "core/include/video_processor_backend_interface.h"
#include "kkrtc_plugin_loader.h"
#include "video_processor_plugin_api.h"
#include "kkrtc_std.h"
#include "kkrtc_plugin_log_observer.h"
#include "kkrtc_plugin_params.h"

namespace kkrtc {
    namespace vpro {
        static std::mutex &getInitializationMutex() {
            static std::mutex initializationMutex;
            return initializationMutex;
        }

        class VideoProcessorPlusinBackend : public IVideoProcessorBackend {
        protected:
            void initVideoProcessorAPI() {
                const char *init_name = "kkrtc_video_processor_plugin_init";
                FN_kkrtc_video_processor_plugin_init_t fn_init = reinterpret_cast<FN_kkrtc_video_processor_plugin_init_t>(dynamicLib_->getSymbol(
                        init_name));
                if (fn_init) {
                    KKLogInfoTag("VideoProcessorPlusinBackend") << "Found entry: '" << init_name << "'";
                    for (int supported_api_version = VPRO_API_VERSION;
                         supported_api_version >= 0; supported_api_version--) {
                        kkrtc_cap_api_ = fn_init(VPRO_ABI_VERSION, supported_api_version, NULL);
                        if (kkrtc_cap_api_)
                            break;
                        KKLogInfoTag("VideoProcessorPlusinBackend")
                            << "Video Processor: plugins is incompatible (can't be initialized): "
                            << dynamicLib_->getName();
                        return;
                    }
                } else {
                    KKLogInfoTag("VideoProcessorPlusinBackend") << "Video Processor: missing plugins init function: '"
                                                                << init_name
                                                                << "', file: "
                                                                << dynamicLib_->getName();
                }
            }

        public:
            std::shared_ptr<kkrtc::plugin::DynamicLib> dynamicLib_;
            const KkVideoProcessorAPI *kkrtc_cap_api_;
        public:
            VideoProcessorPlusinBackend(const KKPtr<kkrtc::plugin::DynamicLib> &lib)
                    : dynamicLib_(lib), kkrtc_cap_api_(nullptr) {
                initVideoProcessorAPI();
            }

            KKPtr<IVideoProcessor> CreateVideoProcessor(const KKMediaFormat &params, kkrtc::KKLogObserver *logObserver,
                                                        kkrtc::PluginGlobParam *plugin_glob_params) const override;
        };


        class VideoProcessorPlusinBackendFactory : public IVideoProcessorBackendFactory {
        public:
            KKVideoProcessorAPIs id_;
            const char *baseName_;
            KKPtr<VideoProcessorPlusinBackend> backend_;
            bool initialized_;
        public:
            VideoProcessorPlusinBackendFactory(KKVideoProcessorAPIs id, const char *baseName) :
                    id_(id), baseName_(baseName),
                    initialized_(false) {
                // nothing, plugins are loaded on demand
            }

            KKPtr<IVideoProcessorBackend> getBackend() const override {
                initBackend();
                return std::static_pointer_cast<IVideoProcessorBackend>(backend_);
            }

            bool isBuiltIn() const override {
                return false;
            }

        protected:
            inline void initBackend() const {
                if (!initialized_) {
                    const_cast<VideoProcessorPlusinBackendFactory *>(this)->initBackend_();
                }
            }

            void initBackend_() {
                std::lock_guard<std::mutex> lock(getInitializationMutex());
                try {
                    if (!initialized_)
                        loadPlugin();
                }
                catch (...) {
                    KKLogErrorTag("VideoProcessorPluginBackendFactory")
                        << "Video Processor: exception during plugins loading: "
                        << baseName_ << ". SKIP";
                }
                initialized_ = true;
            }

            void loadPlugin();
        };

        void VideoProcessorPlusinBackendFactory::loadPlugin() {
            for (const plugin::FileSystemPath_t &plugin: kkrtc::plugin::getPluginCandidates(baseName_)) {
                auto lib = makePtr<kkrtc::plugin::DynamicLib>(plugin);
                if (!lib->isLoaded())
                    continue;
                try {
                    KKPtr<VideoProcessorPlusinBackend> pluginBackend = makePtr<VideoProcessorPlusinBackend>(lib);
                    if (!pluginBackend)
                        return;
                    if (pluginBackend->kkrtc_cap_api_ == NULL
                            ) {
                        KKLogErrorTag("VideoProcessorPlusinBackend") << "pluginBackend->kkrtc_cap_api null";
                        return;
                    }
                    backend_ = pluginBackend;
                    return;
                }
                catch (...) {
                    KKLogWarnTag("VideoProcessorPlusinBackend")
                        << "Video Processor: exception during plugins initialization: "
                        << plugin::toPrintablePath(plugin) << ". SKIP";
                }
            }
        }

        class VideoProcessorPlugin : public kkrtc::vpro::IVideoProcessor {
            const KkVideoProcessorPluginAPI *video_processor_plugin_api;
            KKPtr<kkrtc::PluginGlobParam_t *> plugin_glob_param_;
            KkPluginVideoProcessor kk_video_pro_;
        public:
            ~VideoProcessorPlugin() {
                if (video_processor_plugin_api->release != nullptr &&
                    0 != video_processor_plugin_api->release(kk_video_pro_)) {
                    KKLogErrorTag("VideoProcessorPlugin") << "Video PRO: Can't release capture by plugins '";

                };
                kk_video_pro_ = nullptr;
                if (!plugin_glob_param_.empty()) {
                    plugin_glob_param_.release();
                }
            }

            void Initialize(const kkrtc::KKMediaFormat &mediaFormats) override {

            };

            const KKMediaFormat &GetMediaFormats() const override {
                return kkrtc::KKMediaFormat();
            }

            int GetDomain() const override {
                return video_processor_plugin_api->vpro_id;
            }


            bool IsStarted() const override {
                return kk_video_pro_ != nullptr;
            }

            bool Process(VideoFrame &videoFrame) {
                return video_processor_plugin_api->processor(kk_video_pro_, (void *) &videoFrame);
            }

            VideoProcessorPlugin(const KkVideoProcessorPluginAPI *vpro_plugin_api, KkPluginVideoProcessor kk_vpro)
                    : video_processor_plugin_api(vpro_plugin_api), kk_video_pro_(kk_vpro) {
                assert(video_processor_plugin_api);
                assert(kk_video_pro_);
            }

            static KKPtr<VideoProcessorPlugin> create(const KkVideoProcessorPluginAPI *vpro_plugin_api,
                                                      const KKMediaFormat &mediaFormat,
                                                      kkrtc::KKLogObserver *kkLogObserver,
                                                      PluginGlobParam *plugin_glob_params) {
                assert(vpro_plugin_api);
                KkPluginVideoProcessor kk_vpro = nullptr;

                std::vector<int> vint_params = mediaFormat.getIntVector();
                int *c_params = vint_params.data();
                unsigned n_params = (unsigned) (vint_params.size() / 2);
                plugin_glob_params->c_params = c_params;
                plugin_glob_params->n_params = n_params;
                if (vpro_plugin_api->initialize(&kk_vpro, (void *) plugin_glob_params) != 0) {
                    KKLogErrorTag("video-pro") << " vpro_initialize error.";
                    return nullptr;
                }
                if (vpro_plugin_api->set_log_callback(kk_vpro, kkLogObserver) != 0) {
                    KKLogErrorTag("video-pro") << " set_log_callback error.";
                }
                assert(kk_vpro);
                return makePtr<VideoProcessorPlugin>(vpro_plugin_api, kk_vpro);;
            }
        };

        KKPtr<IVideoProcessor> VideoProcessorPlusinBackend::CreateVideoProcessor(const KKMediaFormat &params,
                                                                                 kkrtc::KKLogObserver *logObserver,
                                                                                 kkrtc::PluginGlobParam *plugin_glob_params) const {
            try {
                if (kkrtc_cap_api_) {
                    return VideoProcessorPlugin::create(kkrtc_cap_api_, params, logObserver, plugin_glob_params);
                }
            }
            catch (...) {
                KKLogErrorTag("kkrtc_vpro_backend_plugin") << "Video Processor: can't create video processor ";
                throw;
            }
            return nullptr;
        }

        KKPtr<IVideoProcessorBackendFactory>
        createVideoProcessorPluginBackendFactory(KKVideoProcessorAPIs id, const char *baseName) {
#if KKRTC_HAVE_FILESYSTEM_SUPPORT && defined(ENABLE_PLUGINS)
            return makePtr<VideoProcessorPlusinBackendFactory>(id, baseName); //.staticCast<IBackendFactory>();
#else
            void(id);
            void(baseName);
            return KKPtr<IVideoProcessorBackendFactory>();
#endif
        };
    }//vpro
}//kkrtc
