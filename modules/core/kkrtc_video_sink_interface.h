//
// Created by devyk on 2023/10/7.
//

#ifndef KKRTC_VIDEO_SINK_INTERFACE_H
#define KKRTC_VIDEO_SINK_INTERFACE_H

namespace kkrtc {
    template <typename VideoFrameT>
    class VideoSinkInterface {
    public:
        virtual ~VideoSinkInterface() = default;

        virtual void OnFrame(const VideoFrameT& frame) = 0;
    };
} //namespace kkrtc
#endif //KKRTC_VIDEO_SINK_INTERFACE_H
