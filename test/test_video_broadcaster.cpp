//
// Created by devyk on 2023/10/20.
//
#include "kkrtc_cap_api.h"
#include "video/video_broadcaster.h"
int main(){
    KKLogConfig_InitGlobLogger(kkrtc::utils::log::LoggerFactory::createLogger(kkrtc::utils::log::LoggerId::SPDLOG));
    KKLogConfig_SetLogLevel(kkrtc::utils::log::LogLevel::TRACE);
    KKLogConfig_SetConsoleOutput(true);
    KKLogConfig_SetFileOutput(true,".");
    KKLogInfo << "test_win_camera_cap";
    kkrtc::KKMediaFormat mediaFormat;
//    mediaFormat.add(kkrtc::vcap::KK_CAP_PROP_FRAME_WIDTH,1920);
//    mediaFormat.add(kkrtc::vcap::KK_CAP_PROP_FRAME_HEIGHT,1080);
//    mediaFormat.add(kkrtc::vcap::KK_CAP_PROP_FPS,30);
    //mediaFormat.add(kkrtc::vcap::KK_CAP_PROP_COLOR_FORMAT,kkrtc::vcap::KKVideoFormat::I420);
    std::shared_ptr<kkrtc::VideoBroadcaster> videoBroadcaster = std::make_shared<kkrtc::VideoBroadcaster>();
    kkrtc::vcap::VideoCapture cap(1,kkrtc::vcap::KK_CAP_DSHOW,mediaFormat);
    cap.RegisterCaptureDataCallback(videoBroadcaster.get());
    if(cap.isOpened())
    {
        KKLogInfo << "test_win_camera_cap open succeed.";
        getchar();
    }
    KKLogInfo << "test_win_camera_cap open fatal.";
    return 0;
}