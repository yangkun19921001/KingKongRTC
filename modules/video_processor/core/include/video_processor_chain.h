//
// Created by devyk on 2023/10/21.
//

#ifndef KKRTC_VIDEO_PROCESSOR_CHAIN_H
#define KKRTC_VIDEO_PROCESSOR_CHAIN_H

#include <vector>
#include <memory>
#include "video_processor_interface.h"

namespace kkrtc {
    namespace vpro {
        namespace plugin {
            // 预处理链
            class VideoProcessorChain {
            private:
                std::vector <std::unique_ptr<IVideoProcessor>> processors;
            public:
                void addProcessor(std::unique_ptr <IVideoProcessor> processor) {
                    processors.push_back(std::move(processor));
                }

                void process(VideoFrame &data) {
                    for (auto &processor: processors) {
                        processor->Process(data);
                    }
                }
            };
        }
    }
}
#endif //KKRTC_VIDEO_PROCESSOR_CHAIN_H
