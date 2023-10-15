//
// Created by devyk on 2023/10/7.
//

#ifndef KKRTC_VIDEO_FRAME_H
#define KKRTC_VIDEO_FRAME_H


#include "kkrtc_export.h"

#include <stdint.h>

#include <utility>

#include "memory/scoped_refptr.h"
#include "video_frame_buffer.h"
#include "video_rotation.h"

namespace kkrtc {
    class RTC_EXPORT VideoFrame {
    public:
        struct RTC_EXPORT UpdateRect {
            int offset_x;
            int offset_y;
            int width;
            int height;

            // Makes this UpdateRect a bounding box of this and other rect.
            void Union(const UpdateRect &other);

            // Makes this UpdateRect an intersection of this and other rect.
            void Intersect(const UpdateRect &other);

            // Sets everything to 0, making this UpdateRect a zero-size (empty) update.
            void MakeEmptyUpdate();

            bool IsEmpty() const;

            // Per-member equality check. Empty rectangles with different offsets would
            // be considered different.
            bool operator==(const UpdateRect &other) const {
                return other.offset_x == offset_x && other.offset_y == offset_y &&
                       other.width == width && other.height == height;
            }

            bool operator!=(const UpdateRect &other) const { return !(*this == other); }

            // Scales update_rect given original frame dimensions.
            // Cropping is applied first, then rect is scaled down.
            // Update rect is snapped to 2x2 grid due to possible UV subsampling and
            // then expanded by additional 2 pixels in each direction to accommodate any
            // possible scaling artifacts.
            // Note, close but not equal update_rects on original frame may result in
            // the same scaled update rects.
           UpdateRect ScaleWithFrame(int frame_width,
                                      int frame_height,
                                      int crop_x,
                                      int crop_y,
                                      int crop_width,
                                      int crop_height,
                                      int scaled_width,
                                      int scaled_height) const;
        };


        // Preferred way of building VideoFrame objects.
        class RTC_EXPORT Builder {
        public:
            Builder();

            ~Builder();

            VideoFrame build();

            Builder &set_video_frame_buffer(
                    const kkrtc::scoped_refptr<VideoFrameBuffer> &buffer);

            Builder &set_timestamp_ms(int64_t timestamp_ms);

            Builder &set_timestamp_us(int64_t timestamp_us);

            Builder &set_timestamp_rtp(uint32_t timestamp_rtp);

            Builder &set_ntp_time_ms(int64_t ntp_time_ms);

            Builder &set_rotation(VideoRotation rotation);

            Builder &set_id(uint16_t id);


        private:
            uint16_t id_ = 0;
            kkrtc::scoped_refptr<kkrtc::VideoFrameBuffer> video_frame_buffer_;
            int64_t timestamp_us_ = 0;
            uint32_t timestamp_rtp_ = 0;
            int64_t ntp_time_ms_ = 0;
            VideoRotation rotation_ = kVideoRotation_0;

        };

        // To be deprecated. Migrate all use to Builder.
        VideoFrame(const kkrtc::scoped_refptr<VideoFrameBuffer> &buffer,
                   kkrtc::VideoRotation rotation,
                   int64_t timestamp_us);

        VideoFrame(const kkrtc::scoped_refptr<VideoFrameBuffer> &buffer,
                   uint32_t timestamp_rtp,
                   int64_t render_time_ms,
                   VideoRotation rotation);

        ~VideoFrame();

        // Support move and copy.
        VideoFrame(const VideoFrame &);

        VideoFrame(VideoFrame &&);

        VideoFrame &operator=(const VideoFrame &);

        VideoFrame &operator=(VideoFrame &&);

        // Get frame width.
        int width() const;

        // Get frame height.
        int height() const;

        // Get frame size in pixels.
        uint32_t size() const;

        // Get frame ID. Returns 0 if ID is not set. Not guaranteed to be transferred
        // from the sender to the receiver, but preserved on the sender side. The id
        // should be propagated between all frame modifications during its lifetime
        // from capturing to sending as encoded image. It is intended to be unique
        // over a time window of a few minutes for the peer connection to which the
        // corresponding video stream belongs to.
        uint16_t id() const { return id_; }

        void set_id(uint16_t id) { id_ = id; }

        // System monotonic clock, same timebase as rtc::TimeMicros().
        int64_t timestamp_us() const { return timestamp_us_; }

        void set_timestamp_us(int64_t timestamp_us) { timestamp_us_ = timestamp_us; }

        // TODO(nisse): After the cricket::VideoFrame and kkrtc::VideoFrame
        // merge, timestamps other than timestamp_us will likely be
        // deprecated.

        // Set frame timestamp (90kHz).
        void set_timestamp(uint32_t timestamp) { timestamp_rtp_ = timestamp; }

        // Get frame timestamp (90kHz).
        uint32_t timestamp() const { return timestamp_rtp_; }

        // For now, transport_frame_id and rtp timestamp are the same.
        // TODO(nisse): Must be handled differently for QUIC.
        uint32_t transport_frame_id() const { return timestamp(); }

        // Set capture ntp time in milliseconds.
        void set_ntp_time_ms(int64_t ntp_time_ms) { ntp_time_ms_ = ntp_time_ms; }

        // Get capture ntp time in milliseconds.
        int64_t ntp_time_ms() const { return ntp_time_ms_; }

        // Naming convention for Coordination of Video Orientation. Please see
        // http://www.etsi.org/deliver/etsi_ts/126100_126199/126114/12.07.00_60/ts_126114v120700p.pdf
        //
        // "pending rotation" or "pending" = a frame that has a VideoRotation > 0.
        //
        // "not pending" = a frame that has a VideoRotation == 0.
        //
        // "apply rotation" = modify a frame from being "pending" to being "not
        //                    pending" rotation (a no-op for "unrotated").
        //
        VideoRotation rotation() const { return rotation_; }

        void set_rotation(VideoRotation rotation) { rotation_ = rotation; }


        // Get render time in milliseconds.
        // TODO(nisse): Deprecated. Migrate all users to timestamp_us().
        int64_t render_time_ms() const;

        // Return the underlying buffer. Never nullptr for a properly
        // initialized VideoFrame.
        kkrtc::scoped_refptr<kkrtc::VideoFrameBuffer> video_frame_buffer() const;

        void set_video_frame_buffer(
                const kkrtc::scoped_refptr<VideoFrameBuffer> &buffer);

        // TODO(nisse): Deprecated.
        // Return true if the frame is stored in a texture.
        bool is_texture() const {
            return video_frame_buffer()->type() == VideoFrameBuffer::Type::kNative;
        }


    private:
        VideoFrame(uint16_t id,
                   const kkrtc::scoped_refptr<VideoFrameBuffer> &buffer,
                   int64_t timestamp_us,
                   uint32_t timestamp_rtp,
                   int64_t ntp_time_ms,
                   VideoRotation rotation
        );

        uint16_t id_;
        // An opaque reference counted handle that stores the pixel data.
        kkrtc::scoped_refptr<kkrtc::VideoFrameBuffer> video_frame_buffer_;
        uint32_t timestamp_rtp_;
        int64_t ntp_time_ms_;
        int64_t timestamp_us_;
        VideoRotation rotation_;

    };

}//namespace KKRTC



#endif //KKRTC_VIDEO_FRAME_H
