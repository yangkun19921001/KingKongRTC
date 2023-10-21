//
// Created by devyk on 2023/10/21.
//

#ifndef KKRTC_DEFAULT_VIDEO_PROCESSOR_H
#define KKRTC_DEFAULT_VIDEO_PROCESSOR_H

#include "core/include/video_processor_interface.h"
#include "kkrtc_plugin_log_observer.h"
#include "core/include/video_processor_plugin_apis.h"
#include "core/include/video_processor_plugin_api.h"

namespace kkrtc {
    namespace plugin {
        namespace def {
            class DefaultVideoPorcessor : public kkrtc::vpro::IVideoProcessor {

            public:
                ~DefaultVideoPorcessor() override;

                DefaultVideoPorcessor();

                /**
                 * 初始化视频处理器
                 * @param mediaFormats 媒体格式
                 */
                void Initialize(const kkrtc::KKMediaFormat &mediaFormats)override;

                /**
                 * 检查处理器是否已启动
                 * @return 是否已启动
                 */
                bool IsStarted()const override;

                /**
                 * 获取处理器域，用于区分不同处理器
                 * @return 域值
                 */
                int GetDomain() const override{ return kkrtc::vpro::KK_VPRO_DEF; }

                /**
                 * 处理视频帧
                 * @param videoFrame 待处理的视频帧
                 * @return 是否成功处理
                 */
                bool Process(VideoFrame &videoFrame)override;

                /**
                 * 获取当前的媒体格式
                 * @return 媒体格式
                 */
                const KKMediaFormat &GetMediaFormats() const override;

            public:
                kkrtc::KKLogObserver *log_callback_;
                kkrtc::utils::log::ILogger *raw_logger_;
                int64_t frames_;
                KKMediaFormat media_format_;

                void SetLogCallback(KKLogObserver *pObserver);
            };
        }//def
    }//plugin
}//kkrtc


#endif //KKRTC_DEFAULT_VIDEO_PROCESSOR_H
