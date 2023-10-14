//
// Created by devyk on 2023/10/9.
//
#include "utils/log_helper.h"
#include "filesystem.h"
#include "kkrtc_cap_backend_interface.h"

class PluginCapBackendFactory;
int main(int argc,char**arg){
    KKLogConfig_InitGlobLogger(kkrtc::utils::log::LoggerFactory::createLogger(kkrtc::utils::log::LoggerId::SPDLOG));
    KKLogConfig_SetLogLevel(kkrtc::utils::log::LogLevel::INFO);
    KKLogConfig_SetConsoleOutput(true);
    KKLogConfig_SetFileOutput(true,".");
    auto cwd = kkrtc::utils::fs::getcwd();
    KKLogInfoTag("cwd") << "cwd:" << cwd ;

    auto plugin = kkrtc::vcap::createPluginBackendFactory(kkrtc::vcap::KK_CAP_DSHOW,"win_dshow");
    plugin->getBackend();
    return 0;
}