//
// Created by devyk on 2023/10/20.
//

#include "video_broadcaster.h"
#include "i420_buffer.h"
#include "utils/log_helper.h"
#include "video/video_common.h"

namespace kkrtc {
    static const char *module_name = "VideoBroadcaster";

    VideoBroadcaster::VideoBroadcaster() = default;

    VideoBroadcaster::~VideoBroadcaster() = default;


    void VideoBroadcaster::AddOrUpdateSink(
            VideoSinkInterface<kkrtc::VideoFrame> *sink,
            const VideoSinkWants &wants) {
        if (sink == nullptr) {
            KKLogErrorTag(module_name) << "sink == null";
            return;
        };
        std::lock_guard<std::mutex> lock(sinks_and_wants_lock_);
        if (!FindSinkPair(sink)) {
            // `Sink` is a new sink, which didn't receive previous frame.
            previous_frame_sent_to_all_sinks_ = false;

            if (last_constraints_.has_value()) {
                KKLogInfoTag(module_name) << __func__ << " forwarding stored constraints min_fps "
                                          << last_constraints_->min_fps.value_or(-1) << " max_fps "
                                          << last_constraints_->max_fps.value_or(-1);
                sink->OnConstraintsChanged(*last_constraints_);
            }
        }
        VideoSourceBase::AddOrUpdateSink(sink, wants);
        UpdateWants();
    }

    void VideoBroadcaster::RemoveSink(
            VideoSinkInterface<kkrtc::VideoFrame> *sink) {
        if (sink == nullptr) {
            KKLogErrorTag(module_name) << "sink == null";
            return;
        };
        std::lock_guard<std::mutex> lock(sinks_and_wants_lock_);
        VideoSourceBase::RemoveSink(sink);
        UpdateWants();
    }

    bool VideoBroadcaster::frame_wanted() const {
        std::lock_guard<std::mutex> lock(sinks_and_wants_lock_);
        return !sink_pairs().empty();
    }

    VideoSinkWants VideoBroadcaster::wants() const {
        std::lock_guard<std::mutex> lock(sinks_and_wants_lock_);
        return current_wants_;
    }

    void VideoBroadcaster::OnFrame(const kkrtc::VideoFrame &frame) {
        std::lock_guard<std::mutex> lock(sinks_and_wants_lock_);
        bool current_frame_was_discarded = false;
        for (auto &sink_pair: sink_pairs()) {
            if (sink_pair.wants.rotation_applied &&
                frame.rotation() != kkrtc::kVideoRotation_0) {
                // Calls to OnFrame are not synchronized with changes to the sink wants.
                // When rotation_applied is set to true, one or a few frames may get here
                // with rotation still pending. Protect sinks that don't expect any
                // pending rotation.
                KKLogInfoTag(module_name) << "Discarding frame with unexpected rotation.";
                sink_pair.sink->OnDiscardedFrame();
                current_frame_was_discarded = true;
                continue;
            }
            if (sink_pair.wants.black_frames) {
                kkrtc::VideoFrame black_frame =
                        kkrtc::VideoFrame::Builder()
                                .set_video_frame_buffer(
                                        GetBlackFrameBuffer(frame.width(), frame.height()))
                                .set_rotation(frame.rotation())
                                .set_timestamp_us(frame.timestamp_us())
                                .set_id(frame.id())
                                .build();
                sink_pair.sink->OnFrame(black_frame);
            } else if (!previous_frame_sent_to_all_sinks_ && frame.has_update_rect()) {
                // Since last frame was not sent to some sinks, no reliable update
                // information is available, so we need to clear the update rect.
                kkrtc::VideoFrame copy = frame;
                copy.clear_update_rect();
                sink_pair.sink->OnFrame(copy);
            } else {
                sink_pair.sink->OnFrame(frame);
            }
        }
        previous_frame_sent_to_all_sinks_ = !current_frame_was_discarded;
    }

    void VideoBroadcaster::OnDiscardedFrame() {
        std::lock_guard<std::mutex> lock(sinks_and_wants_lock_);
        for (auto &sink_pair: sink_pairs()) {
            sink_pair.sink->OnDiscardedFrame();
        }
    }

    void VideoBroadcaster::ProcessConstraints(
            const kkrtc::VideoTrackSourceConstraints &constraints) {
        std::lock_guard<std::mutex> lock(sinks_and_wants_lock_);
        KKLogInfoTag(module_name) << __func__ << " min_fps "
                                  << constraints.min_fps.value_or(-1) << " max_fps "
                                  << constraints.max_fps.value_or(-1) << " broadcasting to "
                                  << sink_pairs().size() << " sinks.";
        last_constraints_ = constraints;
        for (auto &sink_pair: sink_pairs())
            sink_pair.sink->OnConstraintsChanged(constraints);
    }

    void VideoBroadcaster::UpdateWants() {
        VideoSinkWants wants;
        wants.rotation_applied = false;
        wants.resolution_alignment = 1;
        for (auto &sink: sink_pairs()) {
            // wants.rotation_applied == ANY(sink.wants.rotation_applied)
            if (sink.wants.rotation_applied) {
                wants.rotation_applied = true;
            }
            // wants.max_pixel_count == MIN(sink.wants.max_pixel_count)
            if (sink.wants.max_pixel_count < wants.max_pixel_count) {
                wants.max_pixel_count = sink.wants.max_pixel_count;
            }
            // Select the minimum requested target_pixel_count, if any, of all sinks so
            // that we don't over utilize the resources for any one.
            // TODO(sprang): Consider using the median instead, since the limit can be
            // expressed by max_pixel_count.
            if (sink.wants.target_pixel_count &&
                (!wants.target_pixel_count ||
                 (*sink.wants.target_pixel_count < *wants.target_pixel_count))) {
                wants.target_pixel_count = sink.wants.target_pixel_count;
            }
            // Select the minimum for the requested max framerates.
            if (sink.wants.max_framerate_fps < wants.max_framerate_fps) {
                wants.max_framerate_fps = sink.wants.max_framerate_fps;
            }
            wants.resolution_alignment = cricket::LeastCommonMultiple(
                    wants.resolution_alignment, sink.wants.resolution_alignment);
        }

        if (wants.target_pixel_count &&
            *wants.target_pixel_count >= wants.max_pixel_count) {
            wants.target_pixel_count.emplace(wants.max_pixel_count);
        }
        current_wants_ = wants;
    }

    const kkrtc::scoped_refptr<kkrtc::VideoFrameBuffer> &
    VideoBroadcaster::GetBlackFrameBuffer(int width, int height) {
        if (!black_frame_buffer_ || black_frame_buffer_->width() != width ||
            black_frame_buffer_->height() != height) {
            kkrtc::scoped_refptr<kkrtc::I420Buffer> buffer =
                    kkrtc::I420Buffer::Create(width, height);
            kkrtc::I420Buffer::SetBlack(buffer.get());
            black_frame_buffer_ = buffer;
        }

        return black_frame_buffer_;
    }
} // kkrtc