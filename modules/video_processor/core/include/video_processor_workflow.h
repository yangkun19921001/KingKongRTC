//
// Created by devyk on 2023/10/21.
//

#ifndef KKRTC_VIDEO_PROCESSOR_WORKFLOW_H
#define KKRTC_VIDEO_PROCESSOR_WORKFLOW_H

#include "video/video_broadcaster.h"
#include "video_processor_interface.h"
#include "video_processor_chain.h"
namespace kkrtc {
    namespace vpro {
        namespace plugin {
            class VideoProcessorWorkflow {
            public:
                bool SetVideoProcessors(const VideoProcessorChain &chain);

            private:
                std::unique_ptr<IVideoProcessor> video_processor_interface_;
                kkrtc::VideoBroadcaster video_broadcaster_;
            };
        }//pluginn
    }
}//kkrtc



#endif //KKRTC_VIDEO_PROCESSOR_WORKFLOW_H
