//
// Created by devyk on 2023/10/15.
//

#ifndef KKRTC_KKRTC_PLUGIN_LOG_OBSERVER_H
#define KKRTC_KKRTC_PLUGIN_LOG_OBSERVER_H
#include "log_helper.h"
namespace kkrtc {
    class KKLogObserver {
    public:
        virtual ~KKLogObserver() = default;

        virtual void OnLogMessage(kkrtc::utils::log::LogLevel level,const char*name,const char*msg);
    };
}
#endif //KKRTC_KKRTC_PLUGIN_LOG_OBSERVER_H
