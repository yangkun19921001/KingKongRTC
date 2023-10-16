//
// Created by devyk on 2023/10/15.
//

#include "time_utils.h"
#include <chrono>

namespace kkrtc {

    int64_t TimeMillis() {
        auto now = std::chrono::system_clock::now();
        // 转换为毫秒时间戳
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return millis;
    }
}