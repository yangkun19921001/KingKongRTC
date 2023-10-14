//
// Created by devyk on 2023/9/17.
//
#include "kkrtc_cap_backend_interface.h"
#include "kkrtc_plugin_loader.h"
#include "kkrtc_cap_plugin_api.h"
#include "kkrtc_std.h"

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
                                    << "Video I/O: plugin is incompatible (can't be initialized): "
                                    << dynamicLib_->getName();
                            return;
                        }
                    }
                } else {
                    KKLogInfoTag("PluginCapBackend") << "Video I/O: missing plugin init function: '" << init_name
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

            std::shared_ptr<IVideoCapture> createCapture(int camera, const KKMediaFormat &params) const override;

            std::shared_ptr<IVideoCapture> createCapture(const std::string &filename) const;

            std::shared_ptr<IVideoCapture>
            createCapture(const std::string &filename, const KKMediaFormat &params) const override;

        };

    class PluginCapBackendFactory: public ICapBackendFactory {
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
                    KKLogErrorTag("PluginCapBackendFactory") << "Video I/O: exception during plugin loading: "
                                                             << baseName_ << ". SKIP";
                }
                initialized = true;
            }

            void loadPlugin();
        };


        static
        std::vector<plugin::FileSystemPath_t> getPluginCandidates(const std::string& baseName)
        {
            using namespace kkrtc::utils;
            using namespace kkrtc::utils::fs;
            using namespace plugin;
            const std::string baseName_l = toLowerCase(baseName);
            const std::string baseName_u = toUpperCase(baseName);
            const plugin::FileSystemPath_t baseName_l_fs = plugin::toFileSystemPath(baseName_l);
            std::vector<plugin::FileSystemPath_t> paths;
            //todo
            const std::vector<std::string> paths_ /*= getConfigurationParameterPaths("KKRTC_VIDEO_CAP_PLUGIN_PATH", std::vector<std::string>())*/;
            if (paths_.size() != 0)
            {
                for (size_t i = 0; i < paths_.size(); i++)
                {
                    paths.push_back(plugin::toFileSystemPath(paths_[i]));
                }
            }
            else
            {
                FileSystemPath_t binaryLocation;
                //todo
                if (getBinLocation(binaryLocation))
                {
                    binaryLocation = getParent(binaryLocation);
#ifndef KK_VIDEO_CAP_PLUGIN_SUBDIRECTORY
                    paths.push_back(binaryLocation);
#else
#ifdef KKRTC_WIN
                    paths.push_back(binaryLocation + toFileSystemPath("\\") + toFileSystemPath("plugins"));
#else
                    paths.push_back(binaryLocation + toFileSystemPath("/") + toFileSystemPath("plugins"));

#endif
#endif

                }
            }
            const std::string default_expr = plugin::libraryPrefix() + "kkrtc_" + baseName_l /*+ "*"*/ + plugin::librarySuffix();
            //const std::string default_expr = "win_capture_ds_lib.dll";
            //todo
           // const std::string plugin_expr /*= getConfigurationParameterString((std::string("KKRTC_VIDEO_CAP_PLUGIN_") + baseName_u).c_str(), default_expr.c_str())*/;
            const std::string plugin_expr = default_expr;
            std::vector<plugin::FileSystemPath_t> results;
#ifdef _WIN32
            plugin::FileSystemPath_t moduleName = plugin::toFileSystemPath(plugin::libraryPrefix() + "kkrtc_" + baseName_l + plugin::librarySuffix());
            //plugin::FileSystemPath_t moduleName  = plugin::toFileSystemPath("win_capture_ds_lib.dll");

#ifndef WINRT
            if (baseName_u == "FFMPEG")  // backward compatibility
            {
                const wchar_t* ffmpeg_env_path = _wgetenv(L"KKRTC_FFMPEG_DLL_DIR");
                if (ffmpeg_env_path)
                {
                    results.push_back(plugin::FileSystemPath_t(ffmpeg_env_path) + L"\\" + moduleName);
                }
            }
#endif
            if (plugin_expr != default_expr)
            {
                moduleName = plugin::toFileSystemPath(plugin_expr);
                results.push_back(moduleName);
            }
            for (const plugin::FileSystemPath_t& path : paths)
            {
                results.push_back(path + L"\\" + moduleName);
            }

#if defined(_DEBUG) && defined(DEBUG_POSTFIX)
            if (baseName_u == "FFMPEG")  // backward compatibility
    {
        const FileSystemPath_t templ = toFileSystemPath(KKAUX_STR(DEBUG_POSTFIX) ".dll");
        FileSystemPath_t nonDebugName(moduleName);
        size_t suf = nonDebugName.rfind(templ);
        if (suf != FileSystemPath_t::npos)
        {
            nonDebugName.replace(suf, suf + templ.size(), L".dll");
            results.push_back(nonDebugName);
        }
    }
#endif // _DEBUG && DEBUG_POSTFIX
#else
      KKLogInfoTag("kkrtc_cap_backend_plugin")<< "VideoIO plugin (" << baseName << "): glob is '" << plugin_expr << "', " << paths.size() << " location(s)";
    for (const std::string& path : paths)
    {
        if (path.empty())
            continue;
        std::vector<std::string> candidates;
        cv::glob(utils::fs::join(path, plugin_expr), candidates);
        // Prefer candisates with higher versions
        // TODO: implemented accurate versions-based comparator
        std::sort(candidates.begin(), candidates.end(), std::greater<std::string>());
        KKLogInfoTag("kkrtc_cap_backend_plugin")<< "    - " << path << ": " << candidates.size());
        copy(candidates.begin(), candidates.end(), back_inserter(results));
    }
#endif
            KKLogInfoTag("kkrtc_cap_backend_plugin")<<"Found " << results.size() << " plugin(s) for " << baseName;
            return results;
        }

        void PluginCapBackendFactory::loadPlugin() {

            for (const plugin::FileSystemPath_t& plugin : getPluginCandidates(baseName_))
            {
                auto lib = makePtr<kkrtc::plugin::DynamicLib>(plugin);
                if (!lib->isLoaded())
                    continue;
                try
                {
                    KKPtr<PluginCapBackend> pluginBackend = makePtr<PluginCapBackend>(lib);
                    if (!pluginBackend)
                        return;
                    if (pluginBackend->kkrtc_cap_api_ == NULL
                       )
                    {
                        KKLogErrorTag("kkrtc_cap_backend_plugin")<< "pluginBackend->kkrtc_cap_api null";
                        return;
                    }
                    backend = pluginBackend;
                    return;
                }
                catch (...)
                {
                    KKLogWarnTag("kkrtc_cap_backend_plugin")<< "Video I/O: exception during plugin initialization: " << plugin::toPrintablePath(plugin) << ". SKIP";
                }
            }
        }

        class PluginCapture : public kkrtc::vcap::IVideoCapture
        {
            const KKVideoCapturePluginAPI * cap_plugin_api_;
            KkPluginCapture kk_capture_;
        public:
            ~PluginCapture(){
                if (0 != cap_plugin_api_->capture_release(kk_capture_))
                {
                    KKLogErrorTag("kkrtc_cap_backend_plugin")<<"Video I/O: Can't release capture by plugin '";

                };
                kk_capture_ = nullptr;
            }
             void Initialize(const kkrtc::KKMediaFormat& mediaFormats) override {

            };
             bool IsOpened() const override{
                 return kk_capture_ != nullptr;
             };

            const KKMediaFormat& GetMediaFormats() const override{

                return KKMediaFormat();
            }

            int GetCapDomain() const override{ return cap_plugin_api_->cap_id; }

            PluginCapture(const KKVideoCapturePluginAPI* cap_plugin_api, KkPluginCapture kk_capture)
                    : cap_plugin_api_(cap_plugin_api), kk_capture_(kk_capture)
            {
                assert(cap_plugin_api_); assert(kk_capture_);
            }
            static std::shared_ptr<PluginCapture> create(const KKVideoCapturePluginAPI * cap_plugin_api,
                                                         const std::string &filename, int camera, const KKMediaFormat &mediaFormat){
                assert(cap_plugin_api);
                KkPluginCapture kk_capture = nullptr;

                std::vector<int> vint_params = mediaFormat.getIntVector();
                int* c_params = vint_params.data();
                unsigned n_params = (unsigned)(vint_params.size() / 2);
                if (cap_plugin_api->capture_initialize(&kk_capture) != 0)
                {
                    KKLogErrorTag("video-cap") << cap_plugin_api<<" capture_initialize error.";
                    //cap_plugin_api->set_video_sink(kk_capture,)
                }
                if (0 == cap_plugin_api->capture_open_with_params(kk_capture,camera,filename.empty() ? 0 : filename.c_str(),c_params,n_params))
                {
                    assert(kk_capture);
                    return makePtr<PluginCapture>(cap_plugin_api, kk_capture);;
                }
                return makePtr<PluginCapture>(cap_plugin_api, kk_capture);
            }
        };

        std::shared_ptr<IVideoCapture> PluginCapBackend::createCapture(int camera, const KKMediaFormat &params) const {
            try
            {
            if (kkrtc_cap_api_){
                return PluginCapture::create(kkrtc_cap_api_,std::string(),camera,params);
            }
            }
            catch (...)
            {
                KKLogErrorTag("kkrtc_cap_backend_plugin")<<"Video I/O: can't create camera capture: " << camera;
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
        PluginCapBackend::createCapture(const std::string &filename, const KKMediaFormat &params) const {
            try
            {
                if (kkrtc_cap_api_){
                    return PluginCapture::create(kkrtc_cap_api_,filename,0,params);
                }
            }
            catch (...)
            {
                KKLogErrorTag("kkrtc_cap_backend_plugin")<<"Video I/O:can't open file capture: " << filename;
                throw;
            }
            return std::shared_ptr<IVideoCapture>();
        }


        KKPtr<ICapBackendFactory> createPluginBackendFactory(KKVideoCaptureAPIs id, const char* baseName){
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