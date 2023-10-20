//
// Created by devyk on 2023/9/15.
//

#ifndef KKRTC_WIN_CAMERA_CAP_DS_H
#define KKRTC_WIN_CAMERA_CAP_DS_H

#include "kkrtc_cap_interface.h"
#include "video_frame.h"
#include "video_sink_interface.h"
#include "dshowcapture.h"
#include "kkrtc_plugin_log_observer.h"

namespace kkrtc {
    namespace vcap {

        class WinCameraCaptureDS : public IVideoCapture {
        public:
            ~WinCameraCaptureDS() override;

            WinCameraCaptureDS() ;
            WinCameraCaptureDS(int index,KKMediaFormat &mediaFormat);
            void SetVideoCallback(VideoCaptureObserver * videoCaptureObserver);
            void SetLogCallback(kkrtc::KKLogObserver *kk_log_callback);
            void Initialize(const kkrtc::KKMediaFormat &mediaFormats) override;

            bool IsOpened() const override;

            int GetCapDomain() const override;

            const KKMediaFormat &GetMediaFormats() const override ;

            int Open(int index);

        protected:
            void OnVideoData(const DShow::VideoConfig &config, unsigned char *data,
                                                 size_t size, long long startTime,
                                                 long long endTime, long rotation);
            void Close();

            int m_width,m_height,m_framerate ,m_fourcc;
            bool m_auto_focus;
            VideoCaptureObserver* video_callback_;
            KKMediaFormat media_format_;
        public:
            int m_camera_index_;
            DShow::Device device_;
            DShow::VideoConfig videoConfig_;
            kkrtc::KKLogObserver* log_callback_;
            kkrtc::utils::log::ILogger * raw_logger_;

        };

    }//namespace cap
}//namespace kkrtc
#endif //KKRTC_WIN_CAMERA_CAP_DS_H
