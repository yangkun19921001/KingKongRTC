//
// Created by devyk on 2023/10/21.
//

#ifndef KKRTC_VIDEO_PROCESSOR_BACKEND_INTERFACE_H
#define KKRTC_VIDEO_PROCESSOR_BACKEND_INTERFACE_H

#include "video_processor_interface.h"
#include "video_processor_plugin_apis.h"
#include "kkrtc_std_wrapper.h"
#include "kkrtc_plugin_log_observer.h"
#include "kkrtc_plugin_params.h"
#include <memory>
#include <string>

namespace kkrtc {
    namespace vpro {
        class IVideoProcessorBackend {
        public:
            virtual ~IVideoProcessorBackend() {}

            virtual KKPtr<IVideoProcessor>
            CreateVideoProcessor(const KKMediaFormat &params, kkrtc::KKLogObserver *logObserver,
                                 kkrtc::PluginGlobParam *plugin_glob_params) const = 0;
        };

        class IVideoProcessorBackendFactory {
        public:
            virtual ~IVideoProcessorBackendFactory() {}

            virtual KKPtr<IVideoProcessorBackend> getBackend() const = 0;

            virtual bool isBuiltIn() const = 0;
        };

        KKPtr<IVideoProcessorBackendFactory> createVideoProcessorPluginBackendFactory(KKVideoProcessorAPIs id, const char *baseName);
    }
}//kkrtc

#endif //KKRTC_VIDEO_PROCESSOR_BACKEND_INTERFACE_H
