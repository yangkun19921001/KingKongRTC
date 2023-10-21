//
// Created by devyk on 2023/10/8.
//

#ifndef KKRTC_KKRTC_CAP_REGISTRY_H
#define KKRTC_KKRTC_CAP_REGISTRY_H

#include "kkrtc_video_capture_apis.h"
#include "kkrtc_cap_backend_interface.h"
#include "kkrtc_std_wrapper.h"

namespace kkrtc {
    namespace vcap {
        /** Capabilities bitmask */
        enum CapBackendMode {
            MODE_CAPTURE_BY_INDEX = 1 << 0,           //!< device index
            MODE_CAPTURE_BY_FILENAME = 1 << 1,           //!< filename or device path (v4l2)
            MODE_CAPTURE_ALL = MODE_CAPTURE_BY_INDEX + MODE_CAPTURE_BY_FILENAME,
        };
        struct VideoCapBackendInfo {
            KKVideoCaptureAPIs id;
            CapBackendMode mode;
            int priority;     // 1000-<index*10> - default builtin priority
            // 0 - disabled (OPENCV_VIDEOIO_PRIORITY_<name> = 0)
            // >10000 - prioritized list (OPENCV_VIDEOIO_PRIORITY_LIST)
            const char *name;
            KKPtr <ICapBackendFactory> capBackendFactory;
        };
    }

    namespace vcap_registry {
        using namespace kkrtc::vcap;
        std::vector<VideoCapBackendInfo> getAvailableBackends_CaptureByIndex();
        std::vector<VideoCapBackendInfo> getAvailableBackends_CaptureByFilename();
        std::vector<VideoCapBackendInfo> getAvailableBackends_Writer();
        bool checkDeprecatedBackend(int api);

    } // namespace

}


#endif //KKRTC_KKRTC_CAP_REGISTRY_H
