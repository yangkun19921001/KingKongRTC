//
// Created by devyk on 2023/10/7.
//

#include "video_frame.h"
#include "time_utils.h"

#include <algorithm>
#include <utility>

namespace kkrtc {

    void VideoFrame::UpdateRect::Union(const UpdateRect &other) {
        if (other.IsEmpty())
            return;
        if (IsEmpty()) {
            *this = other;
            return;
        }
        int right = std::max(offset_x + width, other.offset_x + other.width);
        int bottom = std::max(offset_y + height, other.offset_y + other.height);
        offset_x = std::min(offset_x, other.offset_x);
        offset_y = std::min(offset_y, other.offset_y);
        width = right - offset_x;
        height = bottom - offset_y;
/*        RTC_DCHECK_GT(width, 0);
        RTC_DCHECK_GT(height, 0);*/
    }

    void VideoFrame::UpdateRect::Intersect(const UpdateRect &other) {
        if (other.IsEmpty() || IsEmpty()) {
            MakeEmptyUpdate();
            return;
        }

        int right = std::min(offset_x + width, other.offset_x + other.width);
        int bottom = std::min(offset_y + height, other.offset_y + other.height);
        offset_x = std::max(offset_x, other.offset_x);
        offset_y = std::max(offset_y, other.offset_y);
        width = right - offset_x;
        height = bottom - offset_y;
        if (width <= 0 || height <= 0) {
            MakeEmptyUpdate();
        }
    }

    void VideoFrame::UpdateRect::MakeEmptyUpdate() {
        width = height = offset_x = offset_y = 0;
    }

    bool VideoFrame::UpdateRect::IsEmpty() const {
        return width == 0 && height == 0;
    }

    VideoFrame::UpdateRect VideoFrame::UpdateRect::ScaleWithFrame(
            int frame_width,
            int frame_height,
            int crop_x,
            int crop_y,
            int crop_width,
            int crop_height,
            int scaled_width,
            int scaled_height) const {
/*        RTC_DCHECK_GT(frame_width, 0);
        RTC_DCHECK_GT(frame_height, 0);

        RTC_DCHECK_GT(crop_width, 0);
        RTC_DCHECK_GT(crop_height, 0);

        RTC_DCHECK_LE(crop_width + crop_x, frame_width);
        RTC_DCHECK_LE(crop_height + crop_y, frame_height);

        RTC_DCHECK_GT(scaled_width, 0);
        RTC_DCHECK_GT(scaled_height, 0);*/

        // Check if update rect is out of the cropped area.
        if (offset_x + width < crop_x || offset_x > crop_x + crop_width ||
            offset_y + height < crop_y || offset_y > crop_y + crop_width) {
            return {0, 0, 0, 0};
        }

        int x = offset_x - crop_x;
        int w = width;
        if (x < 0) {
            w += x;
            x = 0;
        }
        int y = offset_y - crop_y;
        int h = height;
        if (y < 0) {
            h += y;
            y = 0;
        }

        // Lower corner is rounded down.
        x = x * scaled_width / crop_width;
        y = y * scaled_height / crop_height;
        // Upper corner is rounded up.
        w = (w * scaled_width + crop_width - 1) / crop_width;
        h = (h * scaled_height + crop_height - 1) / crop_height;

        // Round to full 2x2 blocks due to possible subsampling in the pixel data.
        if (x % 2) {
            --x;
            ++w;
        }
        if (y % 2) {
            --y;
            ++h;
        }
        if (w % 2) {
            ++w;
        }
        if (h % 2) {
            ++h;
        }

        // Expand the update rect by 2 pixels in each direction to include any
        // possible scaling artifacts.
        if (scaled_width != crop_width || scaled_height != crop_height) {
            if (x > 0) {
                x -= 2;
                w += 2;
            }
            if (y > 0) {
                y -= 2;
                h += 2;
            }
            w += 2;
            h += 2;
        }

        // Ensure update rect is inside frame dimensions.
        if (x + w > scaled_width) {
            w = scaled_width - x;
        }
        if (y + h > scaled_height) {
            h = scaled_height - y;
        }
/*        RTC_DCHECK_GE(w, 0);
        RTC_DCHECK_GE(h, 0);*/
        if (w == 0 || h == 0) {
            w = 0;
            h = 0;
            x = 0;
            y = 0;
        }

        return {x, y, w, h};
    }


    VideoFrame::Builder::Builder() = default;

    VideoFrame::Builder::~Builder() = default;

    VideoFrame VideoFrame::Builder::build() {
//        RTC_CHECK(video_frame_buffer_ != nullptr);
        return VideoFrame(id_, video_frame_buffer_, timestamp_us_, timestamp_rtp_,
                          ntp_time_ms_, rotation_);
    }

    VideoFrame::Builder &VideoFrame::Builder::set_video_frame_buffer(
            const kkrtc::scoped_refptr<VideoFrameBuffer> &buffer) {
        video_frame_buffer_ = buffer;
        return *this;
    }


    VideoFrame::Builder& VideoFrame::Builder::set_timestamp_ms(
            int64_t timestamp_ms) {
        timestamp_us_ = timestamp_ms * kkrtc::kNumMicrosecsPerMillisec;
        return *this;
    }

    VideoFrame::Builder& VideoFrame::Builder::set_timestamp_us(
            int64_t timestamp_us) {
        timestamp_us_ = timestamp_us;
        return *this;
    }

    VideoFrame::Builder& VideoFrame::Builder::set_timestamp_rtp(
            uint32_t timestamp_rtp) {
        timestamp_rtp_ = timestamp_rtp;
        return *this;
    }

    VideoFrame::Builder& VideoFrame::Builder::set_ntp_time_ms(int64_t ntp_time_ms) {
        ntp_time_ms_ = ntp_time_ms;
        return *this;
    }

    VideoFrame::Builder& VideoFrame::Builder::set_rotation(VideoRotation rotation) {
        rotation_ = rotation;
        return *this;
    }


    VideoFrame::Builder& VideoFrame::Builder::set_id(uint16_t id) {
        id_ = id;
        return *this;
    }


    VideoFrame::VideoFrame(const kkrtc::scoped_refptr<VideoFrameBuffer>& buffer,
                           kkrtc::VideoRotation rotation,
                           int64_t timestamp_us)
            : video_frame_buffer_(buffer),
              timestamp_rtp_(0),
              ntp_time_ms_(0),
              timestamp_us_(timestamp_us),
              rotation_(rotation) {}

    VideoFrame::VideoFrame(const kkrtc::scoped_refptr<VideoFrameBuffer>& buffer,
                           uint32_t timestamp_rtp,
                           int64_t render_time_ms,
                           VideoRotation rotation)
            : video_frame_buffer_(buffer),
              timestamp_rtp_(timestamp_rtp),
              ntp_time_ms_(0),
              timestamp_us_(render_time_ms * kkrtc::kNumMicrosecsPerMillisec),
              rotation_(rotation) {
//        RTC_DCHECK(buffer);
    }

    VideoFrame::VideoFrame(uint16_t id,
                           const kkrtc::scoped_refptr<VideoFrameBuffer>& buffer,
                           int64_t timestamp_us,
                           uint32_t timestamp_rtp,
                           int64_t ntp_time_ms,
                           VideoRotation rotation)
            : id_(id),
              video_frame_buffer_(buffer),
              timestamp_rtp_(timestamp_rtp),
              ntp_time_ms_(ntp_time_ms),
              timestamp_us_(timestamp_us),
              rotation_(rotation) {
/*        if (update_rect_) {
            RTC_DCHECK_GE(update_rect_->offset_x, 0);
            RTC_DCHECK_GE(update_rect_->offset_y, 0);
            RTC_DCHECK_LE(update_rect_->offset_x + update_rect_->width, width());
            RTC_DCHECK_LE(update_rect_->offset_y + update_rect_->height, height());
        }*/
    }

    VideoFrame::~VideoFrame() = default;

    VideoFrame::VideoFrame(const VideoFrame&) = default;
    VideoFrame::VideoFrame(VideoFrame&&) = default;
    VideoFrame& VideoFrame::operator=(const VideoFrame&) = default;
    VideoFrame& VideoFrame::operator=(VideoFrame&&) = default;

    int VideoFrame::width() const {
        return video_frame_buffer_ ? video_frame_buffer_->width() : 0;
    }

    int VideoFrame::height() const {
        return video_frame_buffer_ ? video_frame_buffer_->height() : 0;
    }

    uint32_t VideoFrame::size() const {
        return width() * height();
    }

    kkrtc::scoped_refptr<VideoFrameBuffer> VideoFrame::video_frame_buffer() const {
        return video_frame_buffer_;
    }

    void VideoFrame::set_video_frame_buffer(
            const kkrtc::scoped_refptr<VideoFrameBuffer>& buffer) {
//        RTC_CHECK(buffer);
        video_frame_buffer_ = buffer;
    }

    int64_t VideoFrame::render_time_ms() const {
        return timestamp_us() / kkrtc::kNumMicrosecsPerMillisec;
    }
}//namespace kkrtc