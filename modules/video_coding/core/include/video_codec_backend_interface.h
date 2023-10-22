//
// Created by devyk on 2023/10/22.
//

#ifndef KKRTC_VIDEO_CODEC_BACKEND_INTERFACE_H
#define KKRTC_VIDEO_CODEC_BACKEND_INTERFACE_H

#include "video_codec_interface.h"
#include "video_codec_plugin_apis.h"
#include "kkrtc_std_wrapper.h"
#include "kkrtc_plugin_log_observer.h"
#include "kkrtc_plugin_params.h"
#include <memory>
#include <string>

namespace kkrtc {
    namespace codec_core {
        class IVideoCodecBackend {
        public:
            virtual ~IVideoCodecBackend() {}

            virtual KKPtr<IVideoCodec> CreateVideoEncodec(const KKMediaFormat &params, kkrtc::KKLogObserver *logObserver,
                                                          VideoEncodedObserver *videoEncodedObserver,
                                                         kkrtc::PluginGlobParam *plugin_glob_params) const =0;

            virtual KKPtr<IVideoCodec> CreateVideoDecodec(const KKMediaFormat &params, kkrtc::KKLogObserver *logObserver,
                                                          VideoDecodedObserver *videoEncodedObserver,
                                                         kkrtc::PluginGlobParam *plugin_glob_params) const = 0;
        };

        class IVideoCodecBackendFactory {
        public:
            virtual ~IVideoCodecBackendFactory() {}

            virtual KKPtr<IVideoCodecBackend> getBackend() const = 0;

            virtual bool isBuiltIn() const = 0;
        };

        KKPtr<IVideoCodecBackendFactory>
        createVideoCodecPluginBackendFactory(KKVideoCodecAPIs id, const char *baseName);
    }
}//kkrtc

#endif //KKRTC_VIDEO_CODEC_BACKEND_INTERFACE_H
