//
// Created by devyk on 2023/10/22.
//

#ifndef KKRTC_VIDEO_CODEC_INTERFACE_H
#define KKRTC_VIDEO_CODEC_INTERFACE_H

#include "kkrtc_mediaformat.h"
#include "video_frame.h"
#include "video_packet.h"

namespace kkrtc {
    namespace codec_core {
        /**
        * 视频处理接口
        */
        class IVideoCodec {
        public:
            virtual ~IVideoCodec() = default;

            /**
             * 初始化编解码器
             * @param mediaFormats 媒体格式
             */
            virtual void Initialize(const kkrtc::KKMediaFormat &mediaFormats) = 0;

            /**
             * 编码
             */
            virtual int SendFrame(const kkrtc::VideoFrame &videoframe) { return false; };
            /**
             * 解码
             * @return
             */
            virtual int SendPacket(const kkrtc::VideoPacket &videoPacket) { return false; };

            /**
             * 检查处理器是否已启动
             * @return 是否已启动
             */
            virtual bool IsStarted() const = 0;

            /**
             * 获取具体的编解码器
             * @return 域值
             */
            virtual int GetDomain() const { return 0; }

            /**
             * 获取当前的媒体格式
             * @return 媒体格式
             */
            virtual const KKMediaFormat &GetMediaFormats() const = 0;
        };

        class VideoEncodedObserver {
        public:
            virtual ~VideoEncodedObserver() = default;

            virtual void OnEncodedPacket(const VideoPacket &packet) = 0;
        };

        class VideoDecodedObserver {
        public:
            virtual ~VideoDecodedObserver() = default;

            virtual void OnDecodedFrame(const VideoFrame &frame) = 0;
        };
    }//codec_core
}//kkrtc



#endif //KKRTC_VIDEO_CODEC_INTERFACE_H
