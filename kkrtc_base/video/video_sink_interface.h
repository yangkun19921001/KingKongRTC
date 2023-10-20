//
// Created by devyk on 2023/10/7.
//

#ifndef KKRTC_VIDEO_SINK_INTERFACE_H
#define KKRTC_VIDEO_SINK_INTERFACE_H

#include "video/video_track_source_constraints.h"

namespace kkrtc {
    template <typename VideoFrameT>
    class VideoSinkInterface {
    public:
        virtual ~VideoSinkInterface() = default;

        virtual void OnFrame(const VideoFrameT& frame) = 0;

        // Should be called by the source when it discards the frame due to rate
        // limiting.
        virtual void OnDiscardedFrame() {}

        // Called on the network thread when video constraints change.
        // TODO(crbug/1255737): make pure virtual once downstream project adapts.
        virtual void OnConstraintsChanged(
                const kkrtc::VideoTrackSourceConstraints& constraints) {}
    };
} //namespace kkrtc
#endif //KKRTC_VIDEO_SINK_INTERFACE_H
