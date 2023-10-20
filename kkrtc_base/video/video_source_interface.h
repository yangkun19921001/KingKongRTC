//
// Created by devyk on 2023/10/20.
//

#ifndef KKRTC_VIDEO_SOURCE_INTERFACE_H
#define KKRTC_VIDEO_SOURCE_INTERFACE_H

#include <limits>
#include "video/video_sink_interface.h"
#include "abseil-cpp/absl/types/optional.h"

namespace kkrtc {
    struct VideoSinkWants {
        struct FrameSize{
            FrameSize(int width,int height):width(width),height(height){}
            FrameSize(const FrameSize&) =default;
            ~FrameSize() =default;

            int width;
            int height;
        };
        VideoSinkWants();
        VideoSinkWants(const VideoSinkWants&);
        ~VideoSinkWants();
        // Tells the source whether the sink wants frames with rotation applied.
        // By default, any rotation must be applied by the sink.
        bool rotation_applied = false;

        // Tells the source that the sink only wants black frames.
        bool black_frames = false;

        // Tells the source the maximum number of pixels the sink wants.
        int max_pixel_count = std::numeric_limits<int>::max();
        absl::optional<int> target_pixel_count;
        int max_framerate_fps = std::numeric_limits<int>::max();
        int resolution_alignment = 1;
        std::vector<FrameSize> resolutions;
    };

    inline bool operator==(const VideoSinkWants::FrameSize& a,
                           const VideoSinkWants::FrameSize& b) {
        return a.width == b.width && a.height == b.height;
    }

    template <typename VideoFrameT>
    class VideoSourceInterface {
    public:
        virtual ~VideoSourceInterface() = default;
        virtual void AddOrUpdateSink(VideoSinkInterface<VideoFrameT> *sink,const VideoSinkWants&wants) = 0;
        virtual void RemoveSink(VideoSinkInterface<VideoFrameT>*sink) = 0;

    };
}



#endif //KKRTC_VIDEO_SOURCE_INTERFACE_H
