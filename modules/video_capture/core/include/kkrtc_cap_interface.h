//
// Created by devyk on 2023/9/12.
//

#ifndef KKRTC_KKRTC_CAP_INTERFACE_H
#define KKRTC_KKRTC_CAP_INTERFACE_H
#include <vector>
#include "kkrtc_mediaformat.h"
#include "kkrtc_video_capture_apis.h"
namespace kkrtc {
    namespace vcap {
        class IVideoCapture {
        public:
            virtual ~IVideoCapture() = default;
            virtual void Initialize(const kkrtc::KKMediaFormat& mediaFormats) = 0;
            virtual bool IsOpened() const = 0;
            virtual int GetCapDomain() const { return 0; }
            virtual const KKMediaFormat& GetMediaFormats() const = 0;
        };


        class VideoCaptureObserver {
        public:
            virtual ~VideoCaptureObserver() = default;

            virtual void IncomingFrame(uint8_t* videoFrame,
                                       size_t videoFrameLength,
                                       const KKVideoCapConfig& frame, int64_t captureTime = 0) = 0;
        };
    }//namesapce vcap
} // namespace kkrtc

#endif //KKRTC_KKRTC_CAP_INTERFACE_H
