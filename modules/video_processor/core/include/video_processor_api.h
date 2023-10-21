//
// Created by devyk on 2023/10/21.
//
#include "video_processor_api_interface.h"
#include "video/video_broadcaster.h"
#include "video_processor_plugin_apis.h"
#include "kkrtc_plugin_params.h"
#include "kkrtc_plugin_log_observer.h"

namespace kkrtc {
    namespace vpro {
        class VideoProcessor
                : public VideoProcessorModule, public VideoSinkInterface<VideoFrame>, public kkrtc::KKLogObserver {
        public:
            ~VideoProcessor() override;

            VideoProcessor();

            bool IsStarted();

            void Release();

            bool LoadPlugin(const KKVideoProcessorAPIs api);

            bool LoadPlugin(const KKVideoProcessorAPIs api, const kkrtc::KKMediaFormat &format);

            //接收采集的数据或者解码后的数据
            virtual void OnFrame(const kkrtc::VideoFrame &frame) override;

            void AddSink(VideoSinkInterface<kkrtc::VideoFrame> *sink,
                         const VideoSinkWants &wants);

            void RemoveSink(VideoSinkInterface<kkrtc::VideoFrame> *sink);

        protected:
            kkrtc::VideoBroadcaster video_broadcaster_;
            KKPtr<kkrtc::PluginGlobParam_t> plugin_glob_params_;
            std::vector<KKPtr<IVideoProcessor>> video_processors_;
        };


    }
}

