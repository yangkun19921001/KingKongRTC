//
// Created by devyk on 2023/10/8.
//
#include "log_helper.h"
//std::shared_ptr<kkrtc::utils::log::ILogger> globalLogger = kkrtc::utils::log::LoggerFactory::createLogger(kkrtc::utils::log::LoggerId::SPDLOG);

kkrtc::KKPtr<kkrtc::utils::log::ILogger> globalLogger_;

void InitGlobLogger(kkrtc::KKPtr<kkrtc::utils::log::ILogger> logger) {
    globalLogger_ = logger;
}

kkrtc::KKPtr<kkrtc::utils::log::ILogger> GetGlobLogger() {
    if (globalLogger_.empty()) {

    }
    return globalLogger_;
}
