//
// Created by devyk on 2023/10/21.
//

//
// Created by devyk on 2023/9/15.
//
#include <iostream>
#include "kkrtc_cap_api.h"
#include "core/include/video_processor_api.h"

int main(int argc, char **arg) {
    KKLogConfig_InitGlobLogger(kkrtc::utils::log::LoggerFactory::createLogger(kkrtc::utils::log::LoggerId::SPDLOG));
    KKLogConfig_SetLogLevel(kkrtc::utils::log::LogLevel::TRACE);
    KKLogConfig_SetConsoleOutput(true);
    KKLogConfig_SetFileOutput(true, ".");

    kkrtc::KKPtr<kkrtc::vpro::VideoProcessor> videoProcessor = kkrtc::makePtr<kkrtc::vpro::VideoProcessor>();
    videoProcessor->LoadPlugin(kkrtc::vpro::KK_VPRO_ANY);
    if (videoProcessor->IsStarted())
    {
        KKLogInfo << "test_video_processor open succeed.";
    }
    kkrtc::vcap::VideoCapture cap(1, kkrtc::vcap::KK_CAP_ANY, kkrtc::KKMediaFormat());
    cap.RegisterCaptureDataCallback(videoProcessor.get());
    if (cap.isOpened()) {
        KKLogInfo << "test_win_camera_cap open succeed.";
        getchar();
    }
    KKLogInfo << "test_win_camera_cap open fatal.";
    return 0;
}
