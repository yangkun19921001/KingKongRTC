//
// Created by devyk on 2023/10/8.
//
#include "log_helper.h"
#include <mutex>
//std::shared_ptr<kkrtc::utils::log::ILogger> globalLogger = kkrtc::utils::log::LoggerFactory::createLogger(kkrtc::utils::log::LoggerId::SPDLOG);

kkrtc::KKPtr<kkrtc::utils::log::ILogger> globalLogger_;
std::mutex log_lock;

void InitGlobLogger(kkrtc::KKPtr<kkrtc::utils::log::ILogger> logger) {
    std::lock_guard<std::mutex> lock(log_lock);
    globalLogger_ = logger;
}

void InitPluginLogger(kkrtc::KKPtr<kkrtc::utils::log::ILogger> logger) {
    std::lock_guard<std::mutex> lock(log_lock);

}

kkrtc::KKPtr<kkrtc::utils::log::ILogger> GetGlobLogger() {
    std::lock_guard<std::mutex> lock(log_lock);
    return globalLogger_;
}
