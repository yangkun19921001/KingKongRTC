//
// Created by devyk on 2023/10/20.
//

#ifndef KKRTC_VIDEO_TRACK_SOURCE_CONSTRAINTS_H
#define KKRTC_VIDEO_TRACK_SOURCE_CONSTRAINTS_H

#include "abseil-cpp/absl/types/optional.h"

namespace kkrtc {

// This struct definition describes constraints on the video source that may be
// set with VideoTrackSourceInterface::ProcessConstraints.
    struct VideoTrackSourceConstraints {
        absl::optional<double> min_fps;
        absl::optional<double> max_fps;
    };

}  // namespace webrtc
#endif //KKRTC_VIDEO_TRACK_SOURCE_CONSTRAINTS_H
