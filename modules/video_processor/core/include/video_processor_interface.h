//
// Created by devyk on 2023/10/21.
//

#ifndef KKRTC_VIDEO_PROCESSOR_H
#define KKRTC_VIDEO_PROCESSOR_H

#include "kkrtc_mediaformat.h"
#include "video/video_frame.h"

namespace kkrtc {
    namespace vpro {
        /**
         * 视频处理接口
         */
        class IVideoProcessor {
        public:
            virtual ~IVideoProcessor() = default;
            /**
             * 初始化视频处理器
             * @param mediaFormats 媒体格式
             */
            virtual void Initialize(const kkrtc::KKMediaFormat &mediaFormats) = 0;
            /**
             * 检查处理器是否已启动
             * @return 是否已启动
             */
            virtual bool IsStarted() const = 0;
            /**
             * 获取处理器域，用于区分不同处理器
             * @return 域值
             */
            virtual int GetDomain() const { return 0; }
            /**
             * 处理视频帧
             * @param videoFrame 待处理的视频帧
             * @return 是否成功处理
             */
            virtual bool Process(VideoFrame &videoFrame) = 0;
            /**
             * 获取当前的媒体格式
             * @return 媒体格式
             */
            virtual const KKMediaFormat &GetMediaFormats() const = 0;
        };
    } //vpro
} // kkrtc

#endif //KKRTC_VIDEO_PROCESSOR_H
