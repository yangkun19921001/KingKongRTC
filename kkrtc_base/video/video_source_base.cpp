//
// Created by devyk on 2023/10/20.
//

#include "video_source_base.h"
#include "absl/algorithm/container.h"
#include "utils/log_helper.h"

namespace kkrtc {

    static const char *module_name = "VideoSourceBase";
    VideoSourceBase::VideoSourceBase() = default;
    VideoSourceBase::~VideoSourceBase() = default;


    void VideoSourceBase::AddOrUpdateSink(
            VideoSinkInterface<kkrtc::VideoFrame>* sink,
            const VideoSinkWants& wants) {
        if (sink == nullptr)
        {
            KKLogErrorTag(module_name) <<"sink ==null";
            return;
        }
        SinkPair* sink_pair = FindSinkPair(sink);
        if (!sink_pair) {
            sinks_.push_back(SinkPair(sink, wants));
        } else {
            sink_pair->wants = wants;
        }
    }

    void VideoSourceBase::RemoveSink(VideoSinkInterface<kkrtc::VideoFrame>* sink) {
        if (sink == nullptr)
        {
            KKLogErrorTag(module_name) <<"sink ==null";
            return;
        }
        if (!FindSinkPair(sink))
        {
            KKLogErrorTag(module_name) <<"FindSinkPair == null";
            return;
        }
        sinks_.erase(std::remove_if(sinks_.begin(), sinks_.end(),
                                    [sink](const SinkPair& sink_pair) {
                                        return sink_pair.sink == sink;
                                    }),
                     sinks_.end());
    }

    VideoSourceBase::SinkPair* VideoSourceBase::FindSinkPair(
            const VideoSinkInterface<kkrtc::VideoFrame>* sink) {
        auto sink_pair_it = absl::c_find_if(
                sinks_,
                [sink](const SinkPair& sink_pair) { return sink_pair.sink == sink; });
        if (sink_pair_it != sinks_.end()) {
            return &*sink_pair_it;
        }
        return nullptr;
    }

    VideoSourceBaseGuarded::VideoSourceBaseGuarded() = default;
    VideoSourceBaseGuarded::~VideoSourceBaseGuarded() = default;

    void VideoSourceBaseGuarded::AddOrUpdateSink(
            VideoSinkInterface<kkrtc::VideoFrame>* sink,
            const VideoSinkWants& wants) {
        if (sink == nullptr)
        {
            KKLogErrorTag(module_name) <<"sink == null";
            return;
        }
        SinkPair* sink_pair = FindSinkPair(sink);
        if (!sink_pair) {
            sinks_.push_back(SinkPair(sink, wants));
        } else {
            sink_pair->wants = wants;
        }
    }

    void VideoSourceBaseGuarded::RemoveSink(
            VideoSinkInterface<kkrtc::VideoFrame>* sink) {
        if (sink == nullptr)
        {
            KKLogErrorTag(module_name) <<"sink == null";
            return;
        }
        sinks_.erase(std::remove_if(sinks_.begin(), sinks_.end(),
                                    [sink](const SinkPair& sink_pair) {
                                        return sink_pair.sink == sink;
                                    }),
                     sinks_.end());
    }

    VideoSourceBaseGuarded::SinkPair* VideoSourceBaseGuarded::FindSinkPair(
            const VideoSinkInterface<kkrtc::VideoFrame>* sink) {
        auto sink_pair_it = absl::c_find_if(
                sinks_,
                [sink](const SinkPair& sink_pair) { return sink_pair.sink == sink; });
        if (sink_pair_it != sinks_.end()) {
            return &*sink_pair_it;
        }
        return nullptr;
    }

    const std::vector<VideoSourceBaseGuarded::SinkPair>&
    VideoSourceBaseGuarded::sink_pairs() const {
        return sinks_;
    }
}//namespace kkrtc