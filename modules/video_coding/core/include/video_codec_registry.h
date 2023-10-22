//
// Created by devyk on 2023/10/22.
//

#ifndef KKRTC_VIDEO_CODEC_REGISTRY_H
#define KKRTC_VIDEO_CODEC_REGISTRY_H
#include "video_codec_plugin_apis.h"
#include "video_codec_backend_interface.h"
namespace kkrtc {
    namespace codec_core {
        struct VideoCodecBackendInfo {
            KKVideoCodecAPIs id;
            int priority;
            const char *name;
            KKPtr <IVideoCodecBackendFactory> vcodecBackendFactory;
        };
    }

    namespace vcodec_registry {
        using namespace kkrtc::codec_core;
        std::vector<VideoCodecBackendInfo> getAvailableVideoCodecBackends();
        bool checkDeprecatedBackend(int api);

    } // namespace
}//kkrtc
#endif //KKRTC_VIDEO_CODEC_REGISTRY_H
