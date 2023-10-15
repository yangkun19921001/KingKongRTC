/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "video_frame_buffer.h"

#include "i420_buffer.h"
#include "nv12_buffer.h"
#include "ref_counted_object.h"
#include "libyuv/include/libyuv/convert.h"

namespace kkrtc {

// Template to implement a wrapped buffer for a I4??BufferInterface.
    template<typename Base>
    class WrappedYuvBuffer : public Base {
    public:
        WrappedYuvBuffer(int width,
                         int height,
                         const uint8_t *y_plane,
                         int y_stride,
                         const uint8_t *u_plane,
                         int u_stride,
                         const uint8_t *v_plane,
                         int v_stride,
                         std::function<void()> no_longer_used)
                : width_(width),
                  height_(height),
                  y_plane_(y_plane),
                  u_plane_(u_plane),
                  v_plane_(v_plane),
                  y_stride_(y_stride),
                  u_stride_(u_stride),
                  v_stride_(v_stride),
                  no_longer_used_cb_(no_longer_used) {}

        ~WrappedYuvBuffer() override { no_longer_used_cb_(); }

        int width() const override { return width_; }

        int height() const override { return height_; }

        const uint8_t *DataY() const override { return y_plane_; }

        const uint8_t *DataU() const override { return u_plane_; }

        const uint8_t *DataV() const override { return v_plane_; }

        int StrideY() const override { return y_stride_; }

        int StrideU() const override { return u_stride_; }

        int StrideV() const override { return v_stride_; }

    private:
        friend class kkrtc::RefCountedObject<WrappedYuvBuffer>;

        const int width_;
        const int height_;
        const uint8_t *const y_plane_;
        const uint8_t *const u_plane_;
        const uint8_t *const v_plane_;
        const int y_stride_;
        const int u_stride_;
        const int v_stride_;
        std::function<void()> no_longer_used_cb_;
    };

// Template to implement a wrapped buffer for a I4??BufferInterface.
    template<typename BaseWithA>
    class WrappedYuvaBuffer : public WrappedYuvBuffer<BaseWithA> {
    public:
        WrappedYuvaBuffer(int width,
                          int height,
                          const uint8_t *y_plane,
                          int y_stride,
                          const uint8_t *u_plane,
                          int u_stride,
                          const uint8_t *v_plane,
                          int v_stride,
                          const uint8_t *a_plane,
                          int a_stride,
                          std::function<void()> no_longer_used)
                : WrappedYuvBuffer<BaseWithA>(width,
                                              height,
                                              y_plane,
                                              y_stride,
                                              u_plane,
                                              u_stride,
                                              v_plane,
                                              v_stride,
                                              no_longer_used),
                  a_plane_(a_plane),
                  a_stride_(a_stride) {}

        const uint8_t *DataA() const override { return a_plane_; }

        int StrideA() const override { return a_stride_; }

    private:
        const uint8_t *const a_plane_;
        const int a_stride_;
    };

    class I444BufferBase : public I444BufferInterface {
    public:
        kkrtc::scoped_refptr<I420BufferInterface> ToI420() final;
    };

    kkrtc::scoped_refptr<I420BufferInterface> I444BufferBase::ToI420() {
        kkrtc::scoped_refptr<I420Buffer> i420_buffer =
                I420Buffer::Create(width(), height());
        libyuv::I444ToI420(DataY(), StrideY(), DataU(), StrideU(), DataV(), StrideV(),
                           i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                           i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                           i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                           width(), height());
        return i420_buffer;
    }

// Template to implement a wrapped buffer for a PlanarYuv16BBuffer.
    template<typename Base>
    class WrappedYuv16BBuffer : public Base {
    public:
        WrappedYuv16BBuffer(int width,
                            int height,
                            const uint16_t *y_plane,
                            int y_stride,
                            const uint16_t *u_plane,
                            int u_stride,
                            const uint16_t *v_plane,
                            int v_stride,
                            std::function<void()> no_longer_used)
                : width_(width),
                  height_(height),
                  y_plane_(y_plane),
                  u_plane_(u_plane),
                  v_plane_(v_plane),
                  y_stride_(y_stride),
                  u_stride_(u_stride),
                  v_stride_(v_stride),
                  no_longer_used_cb_(no_longer_used) {}

        ~WrappedYuv16BBuffer() override { no_longer_used_cb_(); }

        int width() const override { return width_; }

        int height() const override { return height_; }

        const uint16_t *DataY() const override { return y_plane_; }

        const uint16_t *DataU() const override { return u_plane_; }

        const uint16_t *DataV() const override { return v_plane_; }

        int StrideY() const override { return y_stride_; }

        int StrideU() const override { return u_stride_; }

        int StrideV() const override { return v_stride_; }

    private:
        friend class kkrtc::RefCountedObject<WrappedYuv16BBuffer>;

        const int width_;
        const int height_;
        const uint16_t *const y_plane_;
        const uint16_t *const u_plane_;
        const uint16_t *const v_plane_;
        const int y_stride_;
        const int u_stride_;
        const int v_stride_;
        std::function<void()> no_longer_used_cb_;
    };

    class I010BufferBase : public I010BufferInterface {
    public:
        kkrtc::scoped_refptr<I420BufferInterface> ToI420() final;
    };

    kkrtc::scoped_refptr<I420BufferInterface> I010BufferBase::ToI420() {
        kkrtc::scoped_refptr<I420Buffer> i420_buffer =
                I420Buffer::Create(width(), height());
        libyuv::I010ToI420(DataY(), StrideY(), DataU(), StrideU(), DataV(), StrideV(),
                           i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                           i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                           i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                           width(), height());
        return i420_buffer;
    }

    kkrtc::scoped_refptr<VideoFrameBuffer> VideoFrameBuffer::CropAndScale(
            int offset_x,
            int offset_y,
            int crop_width,
            int crop_height,
            int scaled_width,
            int scaled_height) {
        kkrtc::scoped_refptr<I420Buffer> result =
                I420Buffer::Create(scaled_width, scaled_height);
        result->CropAndScaleFrom(*this->ToI420(), offset_x, offset_y, crop_width,
                                 crop_height);
        return result;
    }

    const I420BufferInterface *VideoFrameBuffer::GetI420() const {
        // Overridden by subclasses that can return an I420 buffer without any
        // conversion, in particular, I420BufferInterface.
        return nullptr;
    }

    const I420ABufferInterface *VideoFrameBuffer::GetI420A() const {
        return static_cast<const I420ABufferInterface *>(this);
    }

    const I444BufferInterface *VideoFrameBuffer::GetI444() const {
        return static_cast<const I444BufferInterface *>(this);
    }

    const I010BufferInterface *VideoFrameBuffer::GetI010() const {
        return static_cast<const I010BufferInterface *>(this);
    }

    const NV12BufferInterface *VideoFrameBuffer::GetNV12() const {
        return static_cast<const NV12BufferInterface *>(this);
    }

    kkrtc::scoped_refptr<VideoFrameBuffer> VideoFrameBuffer::GetMappedFrameBuffer(
            kkrtc::ArrayView<Type> types) {
        return nullptr;
    }

    VideoFrameBuffer::Type I420BufferInterface::type() const {
        return Type::kI420;
    }

    const char *VideoFrameBufferTypeToString(VideoFrameBuffer::Type type) {
        switch (type) {
            case VideoFrameBuffer::Type::kNative:
                return "kNative";
            case VideoFrameBuffer::Type::kI420:
                return "kI420";
            case VideoFrameBuffer::Type::kI420A:
                return "kI420A";
            case VideoFrameBuffer::Type::kI444:
                return "kI444";
            case VideoFrameBuffer::Type::kI010:
                return "kI010";
            case VideoFrameBuffer::Type::kNV12:
                return "kNV12";
            default:
                return nullptr;
        }
    }

    int I420BufferInterface::ChromaWidth() const {
        return (width() + 1) / 2;
    }

    int I420BufferInterface::ChromaHeight() const {
        return (height() + 1) / 2;
    }

    kkrtc::scoped_refptr<I420BufferInterface> I420BufferInterface::ToI420() {
        return this;
    }

    const I420BufferInterface *I420BufferInterface::GetI420() const {
        return this;
    }

    VideoFrameBuffer::Type I420ABufferInterface::type() const {
        return Type::kI420A;
    }

    VideoFrameBuffer::Type I444BufferInterface::type() const {
        return Type::kI444;
    }

    int I444BufferInterface::ChromaWidth() const {
        return width();
    }

    int I444BufferInterface::ChromaHeight() const {
        return height();
    }

    VideoFrameBuffer::Type I010BufferInterface::type() const {
        return Type::kI010;
    }

    int I010BufferInterface::ChromaWidth() const {
        return (width() + 1) / 2;
    }

    int I010BufferInterface::ChromaHeight() const {
        return (height() + 1) / 2;
    }

    VideoFrameBuffer::Type NV12BufferInterface::type() const {
        return Type::kNV12;
    }

    int NV12BufferInterface::ChromaWidth() const {
        return (width() + 1) / 2;
    }

    int NV12BufferInterface::ChromaHeight() const {
        return (height() + 1) / 2;
    }

    kkrtc::scoped_refptr<VideoFrameBuffer> NV12BufferInterface::CropAndScale(
            int offset_x,
            int offset_y,
            int crop_width,
            int crop_height,
            int scaled_width,
            int scaled_height) {
        kkrtc::scoped_refptr<NV12Buffer> result =
                NV12Buffer::Create(scaled_width, scaled_height);
        result->CropAndScaleFrom(*this, offset_x, offset_y, crop_width, crop_height);
        return result;
    }


    kkrtc::scoped_refptr<I420BufferInterface> WrapI420Buffer(
            int width,
            int height,
            const uint8_t *y_plane,
            int y_stride,
            const uint8_t *u_plane,
            int u_stride,
            const uint8_t *v_plane,
            int v_stride,
            std::function<void()> no_longer_used) {
        return kkrtc::scoped_refptr<I420BufferInterface>(
                kkrtc::make_ref_counted<WrappedYuvBuffer<I420BufferInterface>>(
                        width, height, y_plane, y_stride, u_plane, u_stride, v_plane,
                        v_stride, no_longer_used));
    }

    kkrtc::scoped_refptr<I420ABufferInterface> WrapI420ABuffer(
            int width,
            int height,
            const uint8_t *y_plane,
            int y_stride,
            const uint8_t *u_plane,
            int u_stride,
            const uint8_t *v_plane,
            int v_stride,
            const uint8_t *a_plane,
            int a_stride,
            std::function<void()> no_longer_used) {
        return kkrtc::scoped_refptr<I420ABufferInterface>(
                kkrtc::make_ref_counted<WrappedYuvaBuffer<I420ABufferInterface>>(
                        width, height, y_plane, y_stride, u_plane, u_stride, v_plane,
                        v_stride, a_plane, a_stride, no_longer_used));
    }

    kkrtc::scoped_refptr<I444BufferInterface> WrapI444Buffer(
            int width,
            int height,
            const uint8_t *y_plane,
            int y_stride,
            const uint8_t *u_plane,
            int u_stride,
            const uint8_t *v_plane,
            int v_stride,
            std::function<void()> no_longer_used) {
        return kkrtc::scoped_refptr<I444BufferInterface>(
                kkrtc::make_ref_counted<WrappedYuvBuffer<I444BufferBase>>(
                        width, height, y_plane, y_stride, u_plane, u_stride, v_plane,
                        v_stride, no_longer_used));
    }

    kkrtc::scoped_refptr<PlanarYuvBuffer> WrapYuvBuffer(
            VideoFrameBuffer::Type type,
            int width,
            int height,
            const uint8_t *y_plane,
            int y_stride,
            const uint8_t *u_plane,
            int u_stride,
            const uint8_t *v_plane,
            int v_stride,
            std::function<void()> no_longer_used) {
        switch (type) {
            case VideoFrameBuffer::Type::kI420:
                return WrapI420Buffer(width, height, y_plane, y_stride, u_plane, u_stride,
                                      v_plane, v_stride, no_longer_used);
            case VideoFrameBuffer::Type::kI444:
                return WrapI444Buffer(width, height, y_plane, y_stride, u_plane, u_stride,
                                      v_plane, v_stride, no_longer_used);
            default:
//                RTC_CHECK_NOTREACHED();
                break;
        };
    }

    kkrtc::scoped_refptr<I010BufferInterface> WrapI010Buffer(
            int width,
            int height,
            const uint16_t *y_plane,
            int y_stride,
            const uint16_t *u_plane,
            int u_stride,
            const uint16_t *v_plane,
            int v_stride,
            std::function<void()> no_longer_used) {
        return kkrtc::scoped_refptr<I010BufferInterface>(
                kkrtc::make_ref_counted<WrappedYuv16BBuffer<I010BufferBase>>(
                        width, height, y_plane, y_stride, u_plane, u_stride, v_plane,
                        v_stride, no_longer_used));
    }
}  // namespace webrtc
