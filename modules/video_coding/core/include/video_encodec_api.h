//
// Created by devyk on 2023/10/22.
//

#ifndef KKRTC_VIDEO_ENCODEC_API_H
#define KKRTC_VIDEO_ENCODEC_API_H

#include "video/video_sink_interface.h"
#include "video_frame.h"
#include "video_codec_interface.h"
#include "kkrtc_plugin_log_observer.h"
#include "video_codec_plugin_apis.h"
#include "kkrtc_plugin_params.h"

namespace kkrtc {
    namespace vcodec {
        class VideoEncoder
                : public VideoSinkInterface<VideoFrame>,
                  public codec_core::VideoEncodedObserver,
                  public kkrtc::KKLogObserver {
        public:
            VideoEncoder();

            ~VideoEncoder() override;

            void Initialize(KKMediaFormat &mediaFormat);

            /**
             * 当需要编码的帧到来时
             * @param frame
             */
            virtual void OnFrame(const VideoFrame &frame) override;

            /**
             * 编码数据的回调
             * @param packet
             */
            void OnEncodedPacket(const VideoPacket &packet) override;

            bool LoadPlugin(const codec_core::KKVideoCodecAPIs apiPreference);
            bool LoadPlugin(const codec_core::KKVideoCodecAPIs apiPreference,const kkrtc::KKMediaFormat &mediaFormat);
            int RegisterVideoEncodeCompleteObserver(codec_core::VideoEncodedObserver *observer);

            int RemoveVideoEncodeCompleteObserver();

            bool IsStarted();

            bool Release();

        private:
            KKPtr<codec_core::IVideoCodec> video_codec_;
            codec_core::VideoEncodedObserver *encoder_callback_;
            KKPtr <PluginGlobParam_t> plugin_glob_params_;

        };

    }//vcodec
}//kkrtc


#endif //KKRTC_VIDEO_ENCODEC_API_H
