//
// Created by devyk on 2023/10/22.
//

#ifndef KKRTC_FFMPEG_ENC_H
#define KKRTC_FFMPEG_ENC_H

#include "core/include/video_codec_interface.h"
#include "kkrtc_plugin_log_observer.h"

namespace kkrtc {
    namespace vcodec_plugin {
        using namespace kkrtc::codec_core;

        class FFmpegVideoEncoder : public IVideoCodec {
        public:
            FFmpegVideoEncoder();

            ~FFmpegVideoEncoder() override;

            /**
             * 初始化编解码器
             * @param mediaFormats 媒体格式
             */
            virtual void Initialize(const kkrtc::KKMediaFormat &mediaFormats) override;

            /**
             * 编码
             */
            virtual int SendFrame(const kkrtc::VideoFrame &videoframe) override;

            /**
             * 解码
             * @return
             */
            virtual int SendPacket(const kkrtc::VideoPacket &videoPacket) override;

            /**
             * 检查处理器是否已启动
             * @return 是否已启动
             */
            virtual bool IsStarted() const override;

            /**
             * 获取具体的编解码器
             * @return 域值
             */
            virtual int GetDomain() const override ;

            /**
             * 获取当前的媒体格式
             * @return 媒体格式
             */
            virtual const KKMediaFormat &GetMediaFormats() const override;
            void SetLogCallback(KKLogObserver *pLogObserver);
            void SetVideoEncodedCallback(VideoEncodedObserver *pVideoEncodedObserver);

            int Flush();
            int Release();

        private:
            kkrtc::KKLogObserver *log_callback_;
            VideoEncodedObserver *encodec_callback_;
            kkrtc::utils::log::ILogger *raw_logger_;
            kkrtc::KKMediaFormat mediaformt_;
        };
    }
}


#endif //KKRTC_FFMPEG_ENC_H
