//
// Created by devyk on 2023/10/22.
//
#include "core/include/video_codec_registry.h"
#include "utils/filesystem.h"
#include "utils/filesystem_private.h"
#include "utils/kkrtc_std.h"
#include "core/include/video_codec_backend_interface.h"

namespace kkrtc {
    namespace codec_core {
#if KKRTC_HAVE_FILESYSTEM_SUPPORT && defined(ENABLE_PLUGINS)
#define DECLARE_VCODEC_DYNAMIC_BACKEND(api, name) \
{ \
    api, 1000, name, createVideoCodecPluginBackendFactory(api, name) \
},
#else
#define DECLARE_VCODEC_DYNAMIC_BACKEND(cap, name)  /* nothing */
#endif


        static const struct VideoCodecBackendInfo builtin_backends[] =
                {
#ifdef HAVE_FFMPEG
                        DECLARE_VCODEC_DYNAMIC_BACKEND(KK_VCODEC_FFMPEG, "video_codec_ffmpeg")
#endif

                };

        bool sortByPriority(const VideoCodecBackendInfo &lhs, const VideoCodecBackendInfo &rhs) {
            return lhs.priority > rhs.priority;
        }


        class VideoCodecBackendRegistry {
        protected:
            std::vector<VideoCodecBackendInfo> enabledBackends;

            VideoCodecBackendRegistry() {
                const int N = sizeof(builtin_backends) / sizeof(builtin_backends[0]);
                enabledBackends.assign(builtin_backends, builtin_backends + N);
                for (int i = 0; i < N; i++) {
                    VideoCodecBackendInfo &info = enabledBackends[i];
                    info.priority = 1000 - i * 10;
                }
                KKLogDebugTag("kkrtc_video_codec_registry") << "VIDEO CODEC: Builtin backends(" << N << "): "
                                                            << dumpBackends();

                int enabled = 0;
                for (int i = 0; i < N; i++) {
                    VideoCodecBackendInfo &info = enabledBackends[enabled];
                    if (enabled != i)
                        info = enabledBackends[i];
                    size_t param_priority = 1;
                    assert(param_priority == (size_t) (int) param_priority); // overflow check
                    if (param_priority > 0) {
                        info.priority = (int) param_priority;
                        enabled++;
                    } else {
                        KKLogInfoTag("kkrtc_video_codec_registry") << "VIDEO CODEC: Disable backend: " << info.name;
                    }
                }
                enabledBackends.resize(enabled);
                KKLogDebugTag("kkrtc_video_codec_registry") << "VIDEO CODEC: Available backends(" << enabled << "): "
                                                            << dumpBackends();
                std::sort(enabledBackends.begin(), enabledBackends.end(), sortByPriority);
                KKLogInfoTag("kkrtc_video_codec_registry") << "VIDEO CODEC: Enabled backends(" << enabled
                                                           << ", sorted by priority): " << dumpBackends();
            }

        public:
            std::string dumpBackends() const {
                std::ostringstream os;
                for (size_t i = 0; i < enabledBackends.size(); i++) {
                    if (i > 0) os << "; ";
                    const VideoCodecBackendInfo &info = enabledBackends[i];
                    os << info.name << '(' << info.priority << ')';
                }
                return os.str();
            }

            static VideoCodecBackendRegistry &getInstance() {
                static VideoCodecBackendRegistry g_instance;
                return g_instance;
            }

            inline std::vector<VideoCodecBackendInfo> getEnabledBackends() const { return enabledBackends; }

            inline std::vector<VideoCodecBackendInfo> getAvailableVideoCodecBackends() const {
                std::vector<VideoCodecBackendInfo> result;
                for (size_t i = 0; i < enabledBackends.size(); i++) {
                    const VideoCodecBackendInfo &info = enabledBackends[i];
                    result.push_back(info);
                }
                return result;
            }

        };

    }//VCODEC

    namespace vcodec_registry {
        using namespace kkrtc::codec_core;

        std::vector<VideoCodecBackendInfo> getAvailableVideoCodecBackends() {
            const std::vector<VideoCodecBackendInfo> result = VideoCodecBackendRegistry::getInstance().getAvailableVideoCodecBackends();
            return result;
        }

        String getBackendName(KKVideoCodecAPIs api) {
            if (api == KK_VCODEC_ANY)
                return "VCODEC_ANY";  // special case, not a part of backends list
            const int N = sizeof(builtin_backends) / sizeof(builtin_backends[0]);
            for (size_t i = 0; i < N; i++) {
                const VideoCodecBackendInfo &backend = builtin_backends[i];
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

        std::vector<KKVideoCodecAPIs> getBackends() {
            std::vector<VideoCodecBackendInfo> backends = VideoCodecBackendRegistry::getInstance().getEnabledBackends();
            std::vector<KKVideoCodecAPIs> result;
            for (size_t i = 0; i < backends.size(); i++)
                result.push_back((KKVideoCodecAPIs) backends[i].id);
            return result;
        }
    }//codec_registry

}//kkrtc