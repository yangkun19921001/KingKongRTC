//
// Created by devyk on 2023/10/8.
//

#include "kkrtc_cap_registry.h"
#include "filesystem.h"
#include "filesystem_private.h"
#include "kkrtc_cap_backend_interface.h"
#include "kkrtc_std.h"

namespace kkrtc {
    namespace vcap {
#if KKRTC_HAVE_FILESYSTEM_SUPPORT && defined(ENABLE_PLUGINS)
#define DECLARE_DYNAMIC_BACKEND(cap, name, mode) \
{ \
    cap, (CapBackendMode)(mode), 1000, name, createPluginBackendFactory(cap, name) \
},
#else
#define DECLARE_DYNAMIC_BACKEND(cap, name, mode)  /* nothing */
#endif

#define DECLARE_STATIC_BACKEND(cap, name, mode, createCaptureFile, createCaptureCamera, createWriter) \
{ \
    cap, (BackendMode)(mode), 1000, name, createBackendFactory(createCaptureFile, createCaptureCamera, createWriter) \
},

        static const struct VideoCapBackendInfo builtin_backends[] =
                {
#ifdef HAVE_DSHOW
                        DECLARE_DYNAMIC_BACKEND(KK_CAP_DSHOW, "win_dshow", MODE_CAPTURE_BY_INDEX)
#endif

                };

        bool sortByPriority(const VideoCapBackendInfo &lhs, const VideoCapBackendInfo &rhs)
        {
            return lhs.priority > rhs.priority;
        }


/** @brief Manages list of enabled backends
 */
        class VideoBackendRegistry
        {
        protected:
            std::vector<VideoCapBackendInfo> enabledBackends;
            VideoBackendRegistry()
            {
                const int N = sizeof(builtin_backends)/sizeof(builtin_backends[0]);
                enabledBackends.assign(builtin_backends, builtin_backends + N);
                for (int i = 0; i < N; i++)
                {
                    VideoCapBackendInfo& info = enabledBackends[i];
                    info.priority = 1000 - i * 10;
                }
                KKLogDebugTag("kkrtc_cap_registry")<<"VIDEOIO: Builtin backends(" << N << "): " << dumpBackends();
                if (readPrioritySettings())
                {
                    KKLogDebugTag("kkrtc_cap_registry")<< "VIDEOIO: Updated backends priorities: " << dumpBackends();
                }
                int enabled = 0;
                for (int i = 0; i < N; i++)
                {
                    VideoCapBackendInfo& info = enabledBackends[enabled];
                    if (enabled != i)
                        info = enabledBackends[i];
                    size_t param_priority = 1;
                    assert(param_priority == (size_t)(int)param_priority); // overflow check
                    if (param_priority > 0)
                    {
                        info.priority = (int)param_priority;
                        enabled++;
                    }
                    else
                    {
                        KKLogInfoTag("kkrtc_cap_registry")<< "VIDEOIO: Disable backend: " << info.name;
                    }
                }
                enabledBackends.resize(enabled);
                KKLogDebugTag("kkrtc_cap_registry")<< "VIDEOIO: Available backends(" << enabled << "): " << dumpBackends();
                std::sort(enabledBackends.begin(), enabledBackends.end(), sortByPriority);
                KKLogInfoTag("kkrtc_cap_registry")<< "VIDEOIO: Enabled backends(" << enabled << ", sorted by priority): " << dumpBackends();
            }

            static std::vector<std::string> tokenize_string(const std::string& input, char token)
            {
                std::vector<std::string> result;
                std::string::size_type prev_pos = 0, pos = 0;
                while((pos = input.find(token, pos)) != std::string::npos)
                {
                    result.push_back(input.substr(prev_pos, pos-prev_pos));
                    prev_pos = ++pos;
                }
                result.push_back(input.substr(prev_pos));
                return result;
            }
            bool readPrioritySettings()
            {
                return true;
            }
        public:
            std::string dumpBackends() const
            {
                std::ostringstream os;
                for (size_t i = 0; i < enabledBackends.size(); i++)
                {
                    if (i > 0) os << "; ";
                    const VideoCapBackendInfo& info = enabledBackends[i];
                    os << info.name << '(' << info.priority << ')';
                }
                return os.str();
            }

            static VideoBackendRegistry& getInstance()
            {
                static VideoBackendRegistry g_instance;
                return g_instance;
            }

            inline std::vector<VideoCapBackendInfo> getEnabledBackends() const { return enabledBackends; }

            inline std::vector<VideoCapBackendInfo> getAvailableBackends_CaptureByIndex() const
            {
                std::vector<VideoCapBackendInfo> result;
                for (size_t i = 0; i < enabledBackends.size(); i++)
                {
                    const VideoCapBackendInfo& info = enabledBackends[i];
                    if (info.mode & MODE_CAPTURE_BY_INDEX)
                        result.push_back(info);
                }
                return result;
            }
            inline std::vector<VideoCapBackendInfo> getAvailableBackends_CaptureByFilename() const
            {
                std::vector<VideoCapBackendInfo> result;
                for (size_t i = 0; i < enabledBackends.size(); i++)
                {
                    const VideoCapBackendInfo& info = enabledBackends[i];
                    if (info.mode & MODE_CAPTURE_BY_FILENAME)
                        result.push_back(info);
                }
                return result;
            }

        };

    }//namespace vcap

    namespace vcap_registry {
        using namespace kkrtc::vcap;
        std::vector<VideoCapBackendInfo> getAvailableBackends_CaptureByIndex()
        {
            const std::vector<VideoCapBackendInfo> result = VideoBackendRegistry::getInstance().getAvailableBackends_CaptureByIndex();
            return result;
        }
        std::vector<VideoCapBackendInfo> getAvailableBackends_CaptureByFilename()
        {
            const std::vector<VideoCapBackendInfo> result = VideoBackendRegistry::getInstance().getAvailableBackends_CaptureByFilename();
            return result;
        }

        String getBackendName(KKVideoCaptureAPIs api)
        {
            if (api == KK_CAP_ANY)
                return "CAP_ANY";  // special case, not a part of backends list
            const int N = sizeof(builtin_backends)/sizeof(builtin_backends[0]);
            for (size_t i = 0; i < N; i++)
            {
                const VideoCapBackendInfo& backend = builtin_backends[i];
                if (backend.id == api)
                    return backend.name;
            }
#if 0 // deprecated_backends is empty in 5.x for now
            const int M = sizeof(deprecated_backends) / sizeof(deprecated_backends[0]);
    for (size_t i = 0; i < M; i++)
    {
        if (deprecated_backends[i].id == api)
            return deprecated_backends[i].name;
    }
#endif
            return std::format("UnknownVideoAPI(%d)", (int)api);
        }


        std::vector<KKVideoCaptureAPIs> getBackends()
        {
            std::vector<VideoCapBackendInfo> backends = VideoBackendRegistry::getInstance().getEnabledBackends();
            std::vector<KKVideoCaptureAPIs> result;
            for (size_t i = 0; i < backends.size(); i++)
                result.push_back((KKVideoCaptureAPIs)backends[i].id);
            return result;
        }

        std::vector<KKVideoCaptureAPIs> getCameraBackends()
        {
            const std::vector<VideoCapBackendInfo> backends = VideoBackendRegistry::getInstance().getAvailableBackends_CaptureByIndex();
            std::vector<KKVideoCaptureAPIs> result;
            for (size_t i = 0; i < backends.size(); i++)
                result.push_back((KKVideoCaptureAPIs)backends[i].id);
            return result;

        }

        std::vector<KKVideoCaptureAPIs> getStreamBackends()
        {
            const std::vector<VideoCapBackendInfo> backends = VideoBackendRegistry::getInstance().getAvailableBackends_CaptureByFilename();
            std::vector<KKVideoCaptureAPIs> result;
            for (size_t i = 0; i < backends.size(); i++)
                result.push_back((KKVideoCaptureAPIs)backends[i].id);
            return result;

        }


        bool hasBackend(KKVideoCaptureAPIs api)
        {
            std::vector<VideoCapBackendInfo> backends = VideoBackendRegistry::getInstance().getEnabledBackends();
            for (size_t i = 0; i < backends.size(); i++)
            {
                const VideoCapBackendInfo& info = backends[i];
                if (api == info.id)
                {
                    assert(!info.capBackendFactory.empty());
                    return !info.capBackendFactory->getBackend().empty();
                }
            }
            return false;
        }

        bool isBackendBuiltIn(KKVideoCaptureAPIs api)
        {
            std::vector<VideoCapBackendInfo> backends = VideoBackendRegistry::getInstance().getEnabledBackends();
            for (size_t i = 0; i < backends.size(); i++)
            {
                const VideoCapBackendInfo& info = backends[i];
                if (api == info.id)
                {
                    assert(!info.capBackendFactory.empty());
                    return info.capBackendFactory->isBuiltIn();
                }
            }
            return false;
        }

    }//namespace vcap_registry
} //namespace kkrtc
