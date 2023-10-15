//
// Created by devyk on 2023/10/15.
//
#include "kkrtc_plugin_log_observer.h"

void kkrtc::KKLogObserver::OnLogMessage(kkrtc::utils::log::LogLevel level, const char *name, const char *msg) {
    switch (level) {
        case kkrtc::utils::log::LogLevel::TRACE:
            KKLogTraceTag(name) << msg;
            break;
        case kkrtc::utils::log::LogLevel::DEBUG:
            KKLogDebugTag(name) << msg;
            break;
        case kkrtc::utils::log::LogLevel::INFO:
            KKLogInfoTag(name) << msg;
            break;
        case kkrtc::utils::log::LogLevel::WARNING:
            KKLogWarnTag(name) << msg;
            break;
        case kkrtc::utils::log::LogLevel::ERR:
            KKLogErrorTag(name) << msg;
            break;
        default:
            KKLogInfoTag(name) << msg;
            break;
    }
}
