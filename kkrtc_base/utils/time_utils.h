//
// Created by devyk on 2023/10/15.
//

#ifndef KKRTC_TIME_UTILS_H
#define KKRTC_TIME_UTILS_H

#include <stdint.h>
#include <time.h>
#include "kkrtc_export.h"

namespace kkrtc {
    static const int64_t kNumMillisecsPerSec = INT64_C(1000);
    static const int64_t kNumMicrosecsPerSec = INT64_C(1000000);
    static const int64_t kNumNanosecsPerSec = INT64_C(1000000000);

    static const int64_t kNumMicrosecsPerMillisec =
            kNumMicrosecsPerSec / kNumMillisecsPerSec;
    static const int64_t kNumNanosecsPerMillisec =
            kNumNanosecsPerSec / kNumMillisecsPerSec;
    static const int64_t kNumNanosecsPerMicrosec =
            kNumNanosecsPerSec / kNumMicrosecsPerSec;


    RTC_EXPORT int64_t TimeMillis();
}//namespace kkrtc
#endif //KKRTC_TIME_UTILS_H
