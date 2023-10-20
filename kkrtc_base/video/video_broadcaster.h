//
// Created by devyk on 2023/10/20.
//

#ifndef KKRTC_VIDEO_BROADCASTER_H
#define KKRTC_VIDEO_BROADCASTER_H

#include <mutex>
#include "video/video_source_base.h"
#include "video/video_sink_interface.h"
#include "video/video_frame.h"
#include "video_track_source_constraints.h"

namespace kkrtc {

    class VideoBroadcaster : public VideoSourceBase,public VideoSinkInterface<kkrtc::VideoFrame> {
    public:
        VideoBroadcaster();
        ~VideoBroadcaster() override;
        // Adds a new, or updates an already existing sink. If the sink is new and
        // ProcessConstraints has been called previously, the new sink's
        // OnConstraintsCalled method will be invoked with the most recent
        // constraints.
        void AddOrUpdateSink(VideoSinkInterface<kkrtc::VideoFrame>* sink,
                             const VideoSinkWants& wants) override;
        void RemoveSink(VideoSinkInterface<kkrtc::VideoFrame>* sink) override;
        // Returns true if the next frame will be delivered to at least one sink.
        bool frame_wanted() const;
        // Returns VideoSinkWants a source is requested to fulfill. They are
        // aggregated by all VideoSinkWants from all sinks.
        VideoSinkWants wants() const;

        // This method ensures that if a sink sets rotation_applied == true,
        // it will never receive a frame with pending rotation. Our caller
        // may pass in frames without precise synchronization with changes
        // to the VideoSinkWants.
        void OnFrame(const kkrtc::VideoFrame& frame) override;

        void OnDiscardedFrame() override;
        // Called on the network thread when constraints change. Forwards the
        // constraints to sinks added with AddOrUpdateSink via OnConstraintsChanged.
        void ProcessConstraints(
                const kkrtc::VideoTrackSourceConstraints& constraints);

    protected:
        void UpdateWants();
        const kkrtc::scoped_refptr<kkrtc::VideoFrameBuffer>& GetBlackFrameBuffer(
                int width,
                int height);

        mutable std::mutex sinks_and_wants_lock_;
        VideoSinkWants current_wants_;
        kkrtc::scoped_refptr<kkrtc::VideoFrameBuffer> black_frame_buffer_;
        bool previous_frame_sent_to_all_sinks_  = true;
        absl::optional<kkrtc::VideoTrackSourceConstraints> last_constraints_;
    };

} // kkrtc

#endif //KKRTC_VIDEO_BROADCASTER_H
