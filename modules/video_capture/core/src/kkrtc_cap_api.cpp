//
// Created by devyk on 2023/10/8.
//

#include "kkrtc_cap_api.h"
#include "kkrtc_cap_registry.h"
#include "libyuv/kkrtc_libyuv.h"
#include "i420_buffer.h"
#include "libyuv/rotate.h"
#include "libyuv/convert.h"
#include "time_utils.h"

namespace kkrtc {
    namespace vcap {

        VideoCapture::VideoCapture() {

        }

        VideoCapture::~VideoCapture() {

        }

        VideoCapture::VideoCapture(int index, int apiPreference, const KKMediaFormat &params) {
            open(index, apiPreference, params);
        }

        VideoCapture::VideoCapture(const String &filename, int apiPreference, const KKMediaFormat &params) {
            open(filename, apiPreference, params);
        }

        bool VideoCapture::open(const String &filename, int apiPreference, const KKMediaFormat &params) {
            return false;
        }

        bool VideoCapture::open(int index, int apiPreference, const KKMediaFormat &params) {
            if (isOpened()) {
                release();
            }
            const std::vector<VideoCapBackendInfo> backends = kkrtc::vcap_registry::getAvailableBackends_CaptureByIndex();
            for (int i = 0; i < backends.size(); ++i) {
                const VideoCapBackendInfo &info = backends[i];
                if (apiPreference == KK_CAP_ANY || apiPreference == info.id) {
                    if (!info.capBackendFactory) {
                        KKLogDebugTag("kkrtc_cap_api") << "VIDEO CAP(" << info.name
                                                       << "): factory is not available (plugins require filesystem support)";
                        continue;
                    }

                    KKLogDebugTag("kkrtc_cap_api") <<
                                                   std::format("VIDEO CAP(%s): trying capture cameraNum=%d ...",
                                                               info.name, index);
                    assert(!info.capBackendFactory.empty());
                    const KKPtr<ICapBackend> backend = info.capBackendFactory->getBackend();
                    if (!backend.empty()) {
                        try {
                            cap_ = backend->createCapture(index, params, this, this);
                            if (!cap_.empty()) {
                                if (cap_->IsOpened()) {
                                    return true;
                                } else {
                                    KKLogErrorTag("VideoCapture") << "VIDEO CAP(" << info.name
                                                                  << "):  open error. index:" << index;
                                }
                                cap_.release();
                            } else {

                                KKLogErrorTag("VideoCapture") << "VIDEO CAP(" << info.name << "): can't create capture";

                            }

                        } catch (std::exception &e) {
                            KKLogErrorTag("VideoCapture") << "VIDEO CAP(" << info.name << "): raised C++ exception:"
                                                          << e.what();
                        } catch (...) {
                            KKLogErrorTag("VideoCapture") << "VIDEO CAP(" << info.name
                                                          << "): raised unknown C++ exception";

                        }

                    }
                }
            }
            return false;
        }

        bool VideoCapture::isOpened() const {
            return !cap_.empty() && cap_->IsOpened();
        }

        void VideoCapture::release() {
            cap_.release();
        }

        void
        VideoCapture::IncomingFrame(uint8_t *videoFrame, size_t videoFrameLength, const KKVideoCapConfig &frameInfo,
                                    int64_t captureTime) {
            KKLogDebugTag("IncomingFrame") << frameInfo.width << ":" << frameInfo.height << " format:"
                                           << (int) frameInfo.videoType
                                           << "  fps:" << (int) frameInfo.maxFPS
                                           << "  startTime:" << captureTime;

            const int32_t width = frameInfo.width;
            const int32_t height = frameInfo.height;

            // Not encoded, convert to I420.
            if (frameInfo.videoType != KKVideoFormat::MJPEG &&
                CalcBufferSize(frameInfo.videoType, width, abs(height)) !=
                videoFrameLength) {
                KKLogErrorTag("VideoCapture") << "Wrong incoming frame length.";
                return;
            }

            int stride_y = width;
            int stride_uv = (width + 1) / 2;
            int target_width = width;
            int target_height = abs(height);

            bool apply_rotation = false;
            // SetApplyRotation doesn't take any lock. Make a local copy here.
            if (frameInfo.rotation == kVideoRotation_90 ||
                frameInfo.rotation == kVideoRotation_270) {
                target_width = abs(height);
                target_height = width;
                apply_rotation = true;
            }

            kkrtc::scoped_refptr<kkrtc::I420Buffer> buffer = kkrtc::I420Buffer::Create(
                    target_width, target_height, stride_y, stride_uv, stride_uv);
            libyuv::RotationMode rotation_mode = libyuv::kRotate0;
            if (apply_rotation) {
                switch (frameInfo.rotation) {
                    case kVideoRotation_0:
                        rotation_mode = libyuv::kRotate0;
                        break;
                    case kVideoRotation_90:
                        rotation_mode = libyuv::kRotate90;
                        break;
                    case kVideoRotation_180:
                        rotation_mode = libyuv::kRotate180;
                        break;
                    case kVideoRotation_270:
                        rotation_mode = libyuv::kRotate270;
                        break;
                }
            }

            const int conversionResult = libyuv::ConvertToI420(
                    videoFrame, videoFrameLength, buffer.get()->MutableDataY(),
                    buffer.get()->StrideY(), buffer.get()->MutableDataU(),
                    buffer.get()->StrideU(), buffer.get()->MutableDataV(),
                    buffer.get()->StrideV(), 0, 0,  // No Cropping
                    width, height, target_width, target_height, rotation_mode,
                    kkrtc::ConvertKKVideoFormat(frameInfo.videoType));

            if (conversionResult < 0) {
                KKLogErrorTag("VideoCapture") << "Failed to convert capture frame from type "
                                              << static_cast<int>(frameInfo.videoType) << "to I420.";
                return;
            }

            VideoFrame captureFrame =
                    VideoFrame::Builder()
                            .set_video_frame_buffer(buffer)
                            .set_timestamp_rtp(0)
                            .set_timestamp_ms(kkrtc::TimeMillis())
                            .set_rotation(!apply_rotation ? (VideoRotation) frameInfo.rotation : kVideoRotation_0)
                            .build();
            captureFrame.set_ntp_time_ms(captureTime);
        }
    }// namespace vcap
}// namespace kkrtc