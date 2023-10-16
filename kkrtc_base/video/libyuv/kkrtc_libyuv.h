//
// Created by devyk on 2023/10/15.
//

#ifndef KKRTC_KKRTC_LIBYUV_H
#define KKRTC_KKRTC_LIBYUV_H

#include <stdint.h>
#include <stdio.h>

#include <vector>
#include "kkrtc_export.h"
#include "kkrtc_mediaformat.h"
#include "video/video_frame.h"
/*
 * KKRTC's wrapper to libyuv.
 */

namespace kkrtc {

// This is the max PSNR value our algorithms can return.
    const double kPerfectPSNR = 48.0f;

    // Calculate the required buffer size.
    // Input:
    //   - type         :The type of the designated video frame.
    //   - width        :frame width in pixels.
    //   - height       :frame height in pixels.
    // Return value:    :The required size in bytes to accommodate the specified
    //                   video frame.
    size_t CalcBufferSize(KKVideoFormat type, int width, int height);


// Extract buffer from VideoFrame or I420BufferInterface (consecutive
// planes, no stride)
// Input:
//   - frame       : Reference to video frame.
//   - size        : pointer to the size of the allocated buffer. If size is
//                   insufficient, an error will be returned.
//   - buffer      : Pointer to buffer
// Return value: length of buffer if OK, < 0 otherwise.
    int ExtractBuffer(const kkrtc::scoped_refptr <I420BufferInterface> &input_frame,
                      size_t size,
                      uint8_t *buffer);

    int ExtractBuffer(const VideoFrame &input_frame, size_t size, uint8_t *buffer);

// Convert From I420
// Input:
//   - src_frame        : Reference to a source frame.
//   - dst_video_type   : Type of output video.
//   - dst_sample_size  : Required only for the parsing of MJPG.
//   - dst_frame        : Pointer to a destination frame.
// Return value: 0 if OK, < 0 otherwise.
// It is assumed that source and destination have equal height.
    int ConvertFromI420(const VideoFrame &src_frame,
                        KKVideoFormat dst_video_type,
                        int dst_sample_size,
                        uint8_t *dst_frame);

    kkrtc::scoped_refptr <I420BufferInterface> ScaleVideoFrameBuffer(
            const I420BufferInterface &source,
            int dst_width,
            int dst_height);

    double I420SSE(const I420BufferInterface &ref_buffer,
                   const I420BufferInterface &test_buffer);

// Compute PSNR for an I420 frame (all planes).
// Returns the PSNR in decibel, to a maximum of kInfinitePSNR.
    double I420PSNR(const VideoFrame *ref_frame, const VideoFrame *test_frame);

    double I420PSNR(const I420BufferInterface &ref_buffer,
                    const I420BufferInterface &test_buffer);

// Compute SSIM for an I420 frame (all planes).
    double I420SSIM(const VideoFrame *ref_frame, const VideoFrame *test_frame);

    double I420SSIM(const I420BufferInterface &ref_buffer,
                    const I420BufferInterface &test_buffer);

// Helper function for scaling NV12 to NV12.
// If the `src_width` and `src_height` matches the `dst_width` and `dst_height`,
// then `tmp_buffer` is not used. In other cases, the minimum size of
// `tmp_buffer` should be:
//   (src_width/2) * (src_height/2) * 2 + (dst_width/2) * (dst_height/2) * 2
    void NV12Scale(uint8_t *tmp_buffer,
                   const uint8_t *src_y,
                   int src_stride_y,
                   const uint8_t *src_uv,
                   int src_stride_uv,
                   int src_width,
                   int src_height,
                   uint8_t *dst_y,
                   int dst_stride_y,
                   uint8_t *dst_uv,
                   int dst_stride_uv,
                   int dst_width,
                   int dst_height);

// Helper class for directly converting and scaling NV12 to I420. The Y-plane
// will be scaled directly to the I420 destination, which makes this faster
// than separate NV12->I420 + I420->I420 scaling.
    class RTC_EXPORT NV12ToI420Scaler {
    public:
        NV12ToI420Scaler();

        ~NV12ToI420Scaler();

        void NV12ToI420Scale(const uint8_t *src_y,
                             int src_stride_y,
                             const uint8_t *src_uv,
                             int src_stride_uv,
                             int src_width,
                             int src_height,
                             uint8_t *dst_y,
                             int dst_stride_y,
                             uint8_t *dst_u,
                             int dst_stride_u,
                             uint8_t *dst_v,
                             int dst_stride_v,
                             int dst_width,
                             int dst_height);

    private:
        std::vector<uint8_t> tmp_uv_planes_;
    };

// Convert VideoType to libyuv FourCC type
    int ConvertKKVideoFormat(KKVideoFormat video_type);

}//namespace kkrtc


#endif //KKRTC_KKRTC_LIBYUV_H
