//
// Created by devyk on 2023/9/17.
//

#ifndef KKRTC_KKRTC_VIDEO_CAPTURE_APIS_H
#define KKRTC_KKRTC_VIDEO_CAPTURE_APIS_H
#include <stdint.h>
#include "kkrtc_mediaformat.h"
namespace kkrtc {
    namespace vcap {
        enum KKVideoCaptureAPIs {
            KK_CAP_ANY = 0,

            //windows
            KK_CAP_DSHOW = 100,

            //ANDROID
            KK_CAP_ANDROID_NDK=200,
            KK_CAP_ANDROID_JNI,

        };

        enum KKVideoCaptureProperties {
            KK_CAP_PROP_FRAME_WIDTH = 3, //!< 视频流中帧的宽度。
            KK_CAP_PROP_FRAME_HEIGHT = 4, //!< 视频流中帧的高度。
            KK_CAP_PROP_FPS = 5, //!< 帧率。
            KK_CAP_PROP_AUTOFOCUS = 39,//自动聚焦
            KK_CAP_PROP_COLOR_FORMAT = 40,//颜色格式
        };


        struct KKVideoCapConfig {
            int32_t width;
            int32_t height;
            int32_t maxFPS;
            int32_t rotation;
            int cropWidth;
            int cropHeight;
            kkrtc::KKVideoFormat videoType;
            bool interlaced;

            KKVideoCapConfig() {
                width = 0;
                height = 0;
                maxFPS = 0;
                rotation = 0;
                cropWidth = 0;
                cropHeight = 0;
                rotation = 0;
                videoType = kkrtc::KKVideoFormat::Unknown;
                interlaced = false;
            }
            bool operator!=(const KKVideoCapConfig& other) const {
                if (width != other.width)
                    return true;
                if (height != other.height)
                    return true;
                if (maxFPS != other.maxFPS)
                    return true;
                if (videoType != other.videoType)
                    return true;
                if (interlaced != other.interlaced)
                    return true;
                if (rotation != other.rotation)
                    return true;
                if (cropWidth != other.cropWidth)
                    return true;
                if (cropHeight != other.cropHeight)
                    return true;
                return false;
            }
            bool operator==(const KKVideoCapConfig& other) const {
                return !operator!=(other);
            }
        };
    }
}
#endif //KKRTC_KKRTC_VIDEO_CAPTURE_APIS_H
