//
// Created by devyk on 2023/9/18.
//
#include "utils/log_helper.h"
#include "filesystem.h"


int main(int argc,char**arg){
    KKLogConfig_InitGlobLogger(kkrtc::utils::log::LoggerFactory::createLogger(kkrtc::utils::log::LoggerId::SPDLOG));
    KKLogConfig_SetLogLevel(kkrtc::utils::log::LogLevel::INFO);
    KKLogConfig_SetConsoleOutput(true);
    KKLogConfig_SetFileOutput(true,".");
    // 记录不同级别的日志
    KKLogInfo << "message:" << 12.5 << " type:" << 2 ;
    KKLogDebug  << "message:" << 12.5 ;
    KKLogInfo  << "执行了" <<1 ;

    auto cwd = kkrtc::utils::fs::getcwd();
    KKLogInfoTag("cwd") << "cwd:" << cwd ;
    KKLogInfoTag("WinCameraCapture") << "message:" << 12.5 << " type:" << 2 ;
    KKLogDebugTag("WinCameraCapture")  << "message:" << 12.5 ;
    KKLogWarnTag("WinCameraCapture")  << "执行了" << 1 ;
    KKLogErrorTag("WinCameraCapture")  << "执行了" << 1 ;
    return 0;
}