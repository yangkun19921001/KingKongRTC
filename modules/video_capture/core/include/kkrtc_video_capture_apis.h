//
// Created by devyk on 2023/9/17.
//

#ifndef KKRTC_KKRTC_VIDEO_CAPTURE_APIS_H
#define KKRTC_KKRTC_VIDEO_CAPTURE_APIS_H
namespace kkrtc {
    namespace vcap {
        enum KKVideoCaptureAPIs {
            KK_CAP_ANY = 0,

            //windows
            KK_CAP_DSHOW = 1000,

            //ANDROID
            KK_CAP_ANDROID_NDK,
            KK_CAP_ANDROID_JNI,

        };

        enum KKVideoCaptureProperties {
            KK_CAP_PROP_FRAME_WIDTH = 3, //!< 视频流中帧的宽度。
            KK_CAP_PROP_FRAME_HEIGHT = 4, //!< 视频流中帧的高度。
            KK_CAP_PROP_FPS = 5, //!< 帧率。
            KK_CAP_PROP_AUTOFOCUS = 39,//自动聚焦
            KK_CAP_PROP_COLOR_FORMAT = 40,//颜色格式
        };

        enum KKVideoCaptureFormat {

            I420 = 0,
            RGB24,
        };
    }
}
#endif //KKRTC_KKRTC_VIDEO_CAPTURE_APIS_H
