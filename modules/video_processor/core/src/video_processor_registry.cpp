//
// Created by devyk on 2023/10/21.
//
#include "core/include/video_processor_registry.h"
#include "utils/filesystem.h"
#include "utils/filesystem_private.h"
#include "utils/kkrtc_std.h"
#include "video_processor_backend_interface.h"

namespace kkrtc {
    namespace vpro {
#if KKRTC_HAVE_FILESYSTEM_SUPPORT && defined(ENABLE_PLUGINS)
#define DECLARE_VPRO_DYNAMIC_BACKEND(api, name) \
{ \
    api, 1000, name, createVideoProcessorPluginBackendFactory(api, name) \
},
#else
#define DECLARE_VPRO_DYNAMIC_BACKEND(cap, name)  /* nothing */
#endif


        static const struct VideoProcessorBackendInfo builtin_backends[] =
                {
#ifdef HAVE_VPROCESSOR_DEFAULT
                        DECLARE_VPRO_DYNAMIC_BACKEND(KK_VPRO_DEF, "video_processor_default")
#endif

                };

        bool sortByPriority(const VideoProcessorBackendInfo &lhs, const VideoProcessorBackendInfo &rhs) {
            return lhs.priority > rhs.priority;
        }


        class VideoProcessorBackendRegistry {
        protected:
            std::vector<VideoProcessorBackendInfo> enabledBackends;

            VideoProcessorBackendRegistry() {
                const int N = sizeof(builtin_backends) / sizeof(builtin_backends[0]);
                enabledBackends.assign(builtin_backends, builtin_backends + N);
                for (int i = 0; i < N; i++) {
                    VideoProcessorBackendInfo &info = enabledBackends[i];
                    info.priority = 1000 - i * 10;
                }
                KKLogDebugTag("kkrtc_cap_registry") << "VIDEO CAP: Builtin backends(" << N << "): " << dumpBackends();

                int enabled = 0;
                for (int i = 0; i < N; i++) {
                    VideoProcessorBackendInfo &info = enabledBackends[enabled];
                    if (enabled != i)
                        info = enabledBackends[i];
                    size_t param_priority = 1;
                    assert(param_priority == (size_t) (int) param_priority); // overflow check
                    if (param_priority > 0) {
                        info.priority = (int) param_priority;
                        enabled++;
                    } else {
                        KKLogInfoTag("kkrtc_cap_registry") << "VIDEO CAP: Disable backend: " << info.name;
                    }
                }
                enabledBackends.resize(enabled);
                KKLogDebugTag("kkrtc_cap_registry") << "VIDEO CAP: Available backends(" << enabled << "): "
                                                    << dumpBackends();
                std::sort(enabledBackends.begin(), enabledBackends.end(), sortByPriority);
                KKLogInfoTag("kkrtc_cap_registry") << "VIDEO CAP: Enabled backends(" << enabled
                                                   << ", sorted by priority): " << dumpBackends();
            }

        public:
            std::string dumpBackends() const {
                std::ostringstream os;
                for (size_t i = 0; i < enabledBackends.size(); i++) {
                    if (i > 0) os << "; ";
                    const VideoProcessorBackendInfo &info = enabledBackends[i];
                    os << info.name << '(' << info.priority << ')';
                }
                return os.str();
            }

            static VideoProcessorBackendRegistry &getInstance() {
                static VideoProcessorBackendRegistry g_instance;
                return g_instance;
            }

            inline std::vector<VideoProcessorBackendInfo> getEnabledBackends() const { return enabledBackends; }

            inline std::vector<VideoProcessorBackendInfo> getAvailableVideoProcessorBackends() const {
                std::vector<VideoProcessorBackendInfo> result;
                for (size_t i = 0; i < enabledBackends.size(); i++) {
                    const VideoProcessorBackendInfo &info = enabledBackends[i];
                    result.push_back(info);
                }
                return result;
            }

        };

    }//vpro

    namespace vpro_registry {
        using namespace kkrtc::vpro;

        std::vector<VideoProcessorBackendInfo> getAvailableVideoProcessorBackends() {
            const std::vector<VideoProcessorBackendInfo> result = VideoProcessorBackendRegistry::getInstance().getAvailableVideoProcessorBackends();
            return result;
        }

        String getBackendName(KKVideoProcessorAPIs api) {
            if (api == KK_VPRO_ANY)
                return "VPRO_ANY";  // special case, not a part of backends list
            const int N = sizeof(builtin_backends) / sizeof(builtin_backends[0]);
            for (size_t i = 0; i < N; i++) {
                const VideoProcessorBackendInfo &backend = builtin_backends[i];
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
            return std::format("UnknownVideoAPI(%d)", (int) api);
        }

        std::vector<KKVideoProcessorAPIs> getBackends() {
            std::vector<VideoProcessorBackendInfo> backends = VideoProcessorBackendRegistry::getInstance().getEnabledBackends();
            std::vector<KKVideoProcessorAPIs> result;
            for (size_t i = 0; i < backends.size(); i++)
                result.push_back((KKVideoProcessorAPIs) backends[i].id);
            return result;
        }
    }//vpro_registry

}//kkrtc