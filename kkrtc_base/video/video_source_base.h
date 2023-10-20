//
// Created by devyk on 2023/10/20.
//

#ifndef KKRTC_VIDEO_SOURCE_BASE_H
#define KKRTC_VIDEO_SOURCE_BASE_H

#include "video/video_source_interface.h"
#include "video/video_frame.h"
namespace kkrtc {
    class VideoSourceBase :public VideoSourceInterface<kkrtc::VideoFrame>{
    public:
        VideoSourceBase();
        ~VideoSourceBase() override;
        void AddOrUpdateSink(VideoSinkInterface<kkrtc::VideoFrame>* sink,
                             const VideoSinkWants& wants) override;
        void RemoveSink(VideoSinkInterface<kkrtc::VideoFrame>* sink) override;
    protected:
        struct SinkPair {
            SinkPair(VideoSinkInterface<kkrtc::VideoFrame>* sink, VideoSinkWants wants)
                    : sink(sink), wants(wants) {}
            VideoSinkInterface<kkrtc::VideoFrame>* sink;
            VideoSinkWants wants;
        };
        SinkPair* FindSinkPair(const VideoSinkInterface<kkrtc::VideoFrame>* sink);

        const std::vector<SinkPair>& sink_pairs() const { return sinks_; }

    private:
        std::vector<SinkPair> sinks_;
    };


// VideoSourceBaseGuarded assumes that operations related to sinks, occur on the
// same TQ/thread that the object was constructed on.
    class VideoSourceBaseGuarded : public VideoSourceInterface<kkrtc::VideoFrame> {
    public:
        VideoSourceBaseGuarded();
        ~VideoSourceBaseGuarded() override;

        void AddOrUpdateSink(VideoSinkInterface<kkrtc::VideoFrame>* sink,
                             const VideoSinkWants& wants) override;
        void RemoveSink(VideoSinkInterface<kkrtc::VideoFrame>* sink) override;

    protected:
        struct SinkPair {
            SinkPair(VideoSinkInterface<kkrtc::VideoFrame>* sink, VideoSinkWants wants)
                    : sink(sink), wants(wants) {}
            VideoSinkInterface<kkrtc::VideoFrame>* sink;
            VideoSinkWants wants;
        };

        SinkPair* FindSinkPair(const VideoSinkInterface<kkrtc::VideoFrame>* sink);
        const std::vector<SinkPair>& sink_pairs() const;

    private:
        std::vector<SinkPair> sinks_;
    };

}



#endif //KKRTC_VIDEO_SOURCE_BASE_H
