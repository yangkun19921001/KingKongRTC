//
// Created by devyk on 2023/9/15.
//
#include <iostream>
#include "kkrtc_cap_api.h"

int main(int argc,char**arg){
    KKLogConfig_InitGlobLogger(kkrtc::utils::log::LoggerFactory::createLogger(kkrtc::utils::log::LoggerId::SPDLOG));
    KKLogConfig_SetLogLevel(kkrtc::utils::log::LogLevel::DEBUG);
    KKLogConfig_SetConsoleOutput(true);
    KKLogConfig_SetFileOutput(true,".");

    KKLogInfo << "test_win_camera_cap";
    kkrtc::KKMediaFormat mediaFormat;
    mediaFormat.add(kkrtc::vcap::KK_CAP_PROP_FRAME_WIDTH,1920);
    mediaFormat.add(kkrtc::vcap::KK_CAP_PROP_FRAME_HEIGHT,1080);
    mediaFormat.add(kkrtc::vcap::KK_CAP_PROP_FPS,30);
    mediaFormat.add(kkrtc::vcap::KK_CAP_PROP_COLOR_FORMAT,kkrtc::vcap::KKVideoCaptureFormat::I420);
    kkrtc::vcap::VideoCapture cap(0,kkrtc::vcap::KK_CAP_DSHOW,mediaFormat);
    if(cap.isOpened())
    {
        KKLogInfo << "test_win_camera_cap open succeed.";
        getchar();
    }
    KKLogInfo << "test_win_camera_cap open fatal.";
    return 0;
}