//
// Created by devyk on 2023/9/17.
//
#include "kkrtc_cap_backend_interface.h"
#include "kkrtc_plugin_loader.h"
#include "kkrtc_cap_plugin_api.h"
#include "kkrtc_std.h"
#include "kkrtc_plugin_log_observer.h"

namespace kkrtc {
    namespace vcap {

        static std::mutex &getInitializationMutex() {
            static std::mutex initializationMutex;
            return initializationMutex;
        }

        class PluginCapBackend : public ICapBackend {
        protected:
            void initCaptureAPI() {
                const char *init_name = "kkrtc_video_capture_plugin_init";
                FN_kkrtc_video_capture_plugin_init_t fn_init = reinterpret_cast<FN_kkrtc_video_capture_plugin_init_t>(dynamicLib_->getSymbol(
                        init_name));
                if (fn_init) {
                    KKLogInfoTag("PluginCapBackend") << "Found entry: '" << init_name << "'";
                    for (int supported_api_version = CAPTURE_API_VERSION;
                         supported_api_version >= 0; supported_api_version--) {
                        kkrtc_cap_api_ = fn_init(CAPTURE_ABI_VERSION, supported_api_version, NULL);
                        if (kkrtc_cap_api_)
                            break;
                        if (!kkrtc_cap_api_) {
                            KKLogInfoTag("PluginCapBackend")
                                << "Video CAP: plugin is incompatible (can't be initialized): "
                                << dynamicLib_->getName();
                            return;
                        }
                    }
                } else {
                    KKLogInfoTag("PluginCapBackend") << "Video CAP: missing plugin init function: '" << init_name
                                                     << "', file: "
                                                     << dynamicLib_->getName();
                }
            }

        public:
            std::shared_ptr<kkrtc::plugin::DynamicLib> dynamicLib_;
            const KKVideoCaptureAPI *kkrtc_cap_api_;
        public:
            PluginCapBackend(const KKPtr<kkrtc::plugin::DynamicLib> &lib)
                    : dynamicLib_(lib), kkrtc_cap_api_(nullptr) {
                initCaptureAPI();
            }

            std::shared_ptr<IVideoCapture> createCapture(int camera) const;

            std::shared_ptr<IVideoCapture>
            createCapture(int camera, const KKMediaFormat &params, VideoCaptureObserver *capobserver,
                          kkrtc::KKLogObserver *logObserver,void*plugin_glob_params) const override;

            std::shared_ptr<IVideoCapture> createCapture(const std::string &filename) const;

            std::shared_ptr<IVideoCapture>
            createCapture(const std::string &filename, const KKMediaFormat &params, VideoCaptureObserver *capobserver,
                          kkrtc::KKLogObserver *logObserver,void*plugin_glob_params) const override;

        };

        class PluginCapBackendFactory : public ICapBackendFactory {
        public:
            KKVideoCaptureAPIs id_;
            const char *baseName_;
            std::shared_ptr<PluginCapBackend> backend;
            bool initialized;
        public:
            PluginCapBackendFactory(KKVideoCaptureAPIs id, const char *baseName) :
                    id_(id), baseName_(baseName),
                    initialized(false) {
                // nothing, plugins are loaded on demand
            }

            KKPtr<ICapBackend> getBackend() const override {
                initBackend();
                return std::static_pointer_cast<ICapBackend>(backend);
            }

            bool isBuiltIn() const override {
                return false;
            }

        protected:
            inline void initBackend() const {
                if (!initialized) {
                    const_cast<PluginCapBackendFactory *>(this)->initBackend_();
                }
            }

            void initBackend_() {
                std::lock_guard<std::mutex> lock(getInitializationMutex());
                try {
                    if (!initialized)
                        loadPlugin();
                }
                catch (...) {
                    KKLogErrorTag("PluginCapBackendFactory") << "Video CAP: exception during plugin loading: "
                                                             << baseName_ << ". SKIP";
                }
                initialized = true;
            }

            void loadPlugin();
        };

        void PluginCapBackendFactory::loadPlugin() {

            for (const plugin::FileSystemPath_t &plugin: kkrtc::plugin::getPluginCandidates(baseName_)) {
                auto lib = makePtr<kkrtc::plugin::DynamicLib>(plugin);
                if (!lib->isLoaded())
                    continue;
                try {
                    KKPtr<PluginCapBackend> pluginBackend = makePtr<PluginCapBackend>(lib);
                    if (!pluginBackend)
                        return;
                    if (pluginBackend->kkrtc_cap_api_ == NULL
                            ) {
                        KKLogErrorTag("kkrtc_cap_backend_plugin") << "pluginBackend->kkrtc_cap_api null";
                        return;
                    }
                    backend = pluginBackend;
                    return;
                }
                catch (...) {
                    KKLogWarnTag("kkrtc_cap_backend_plugin") << "Video CAP: exception during plugin initialization: "
                                                             << plugin::toPrintablePath(plugin) << ". SKIP";
                }
            }
        }

        class PluginCapture : public kkrtc::vcap::IVideoCapture {
            const KKVideoCapturePluginAPI *cap_plugin_api_;
            KKPtr<kkrtc::PluginGlobParam_t *> plugin_glob_param_;
            KkPluginCapture kk_capture_;
        public:
            ~PluginCapture() {
                if (0 != cap_plugin_api_->capture_release(kk_capture_)) {
                    KKLogErrorTag("kkrtc_cap_backend_plugin") << "Video CAP: Can't release capture by plugin '";

                };
                kk_capture_ = nullptr;
                if (!plugin_glob_param_.empty()) {
                    plugin_glob_param_.release();
                }
            }

            void Initialize(const kkrtc::KKMediaFormat &mediaFormats) override {

            };

            bool IsOpened() const override {
                return kk_capture_ != nullptr;
            };

            const KKMediaFormat &GetMediaFormats() const override {

                return KKMediaFormat();
            }

            int GetCapDomain() const override { return cap_plugin_api_->cap_id; }

            PluginCapture(const KKVideoCapturePluginAPI *cap_plugin_api, KkPluginCapture kk_capture)
                    : cap_plugin_api_(cap_plugin_api), kk_capture_(kk_capture) {
                assert(cap_plugin_api_);
                assert(kk_capture_);
            }

            static std::shared_ptr<PluginCapture> create(const KKVideoCapturePluginAPI *cap_plugin_api,
                                                         const std::string &filename, int camera,
                                                         const KKMediaFormat &mediaFormat,
                                                         VideoCaptureObserver *captureObserver,
                                                         kkrtc::KKLogObserver *kkLogObserver,
                                                         void *plugin_glob_params) {
                assert(cap_plugin_api);
                KkPluginCapture kk_capture = nullptr;

                std::vector<int> vint_params = mediaFormat.getIntVector();
                int *c_params = vint_params.data();
                unsigned n_params = (unsigned) (vint_params.size() / 2);
                if (cap_plugin_api->capture_initialize(&kk_capture,plugin_glob_params) != 0) {
                    KKLogErrorTag("video-cap") << " capture_initialize error.";
                    return nullptr;
                }

                if (cap_plugin_api->set_log_callback(kk_capture, kkLogObserver) != 0) {
                    KKLogErrorTag("video-cap") << " set_log_callback error.";
                }

                if (cap_plugin_api->set_video_callback(kk_capture, captureObserver) != 0) {
                    KKLogErrorTag("video-cap") << " set_video_callback error.";
                }
                if (0 == cap_plugin_api->capture_open_with_params(kk_capture, camera,
                                                                  filename.empty() ? 0 : filename.c_str(), c_params,
                                                                  n_params)) {
                    assert(kk_capture);
                    return makePtr<PluginCapture>(cap_plugin_api, kk_capture);;
                }
                return nullptr;
            }
        };

        std::shared_ptr<IVideoCapture>
        PluginCapBackend::createCapture(int camera, const KKMediaFormat &params, VideoCaptureObserver *capobserver,
                                        kkrtc::KKLogObserver *logObserver, void *plugin_glob_params) const {
            try {
                if (kkrtc_cap_api_) {
                    return PluginCapture::create(kkrtc_cap_api_, std::string(), camera, params, capobserver,
                                                 logObserver,plugin_glob_params);
                }
            }
            catch (...) {
                KKLogErrorTag("kkrtc_cap_backend_plugin") << "Video CAP: can't create camera capture: " << camera;
                throw;
            }
            return KKPtr<IVideoCapture>();
        }

        std::shared_ptr<IVideoCapture> PluginCapBackend::createCapture(int camera) const {
            return std::shared_ptr<IVideoCapture>();
        }

        std::shared_ptr<IVideoCapture> PluginCapBackend::createCapture(const std::string &filename) const {
            return std::shared_ptr<IVideoCapture>();
        }

        std::shared_ptr<IVideoCapture>
        PluginCapBackend::createCapture(const std::string &filename, const KKMediaFormat &params,
                                        VideoCaptureObserver *capobserver, kkrtc::KKLogObserver *logObserver,void*plugin_glob_params) const {
            try {
                if (kkrtc_cap_api_) {
                    return PluginCapture::create(kkrtc_cap_api_, filename, 0, params, capobserver, logObserver,plugin_glob_params);
                }
            }
            catch (...) {
                KKLogErrorTag("kkrtc_cap_backend_plugin") << "Video CAP:can't open file capture: " << filename;
                throw;
            }
            return std::shared_ptr<IVideoCapture>();
        }


        KKPtr<ICapBackendFactory> createPluginBackendFactory(KKVideoCaptureAPIs id, const char *baseName) {
#if KKRTC_HAVE_FILESYSTEM_SUPPORT && defined(ENABLE_PLUGINS)
            return makePtr<PluginCapBackendFactory>(id, baseName); //.staticCast<IBackendFactory>();
#else
            void(id);
            void(baseName);
            return KKPtr<ICapBackendFactory>();
#endif
        };
    }
}