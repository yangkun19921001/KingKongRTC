//
// Created by devyk on 2023/10/21.
//

#ifndef KKRTC_VIDEO_PROCESSOR_REGISTRY_H
#define KKRTC_VIDEO_PROCESSOR_REGISTRY_H
#include "video_processor_plugin_apis.h"
#include "video_processor_backend_interface.h"
namespace kkrtc {
    namespace vpro {
        struct VideoProcessorBackendInfo {
            KKVideoProcessorAPIs id;
            int priority;
            const char *name;
            KKPtr <IVideoProcessorBackendFactory> vproBackendFactory;
        };
    }

    namespace vpro_registry {
        using namespace kkrtc::vpro;
        std::vector<VideoProcessorBackendInfo> getAvailableVideoProcessorBackends();
        bool checkDeprecatedBackend(int api);

    } // namespace
}//kkrtc
#endif //KKRTC_VIDEO_PROCESSOR_REGISTRY_H
