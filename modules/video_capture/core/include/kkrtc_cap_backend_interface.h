//
// Created by devyk on 2023/9/17.
//

#ifndef KKRTC_KKRTC_CAP_BACKEND_INTERFACE_H
#define KKRTC_KKRTC_CAP_BACKEND_INTERFACE_H

#include "kkrtc_cap_interface.h"
#include "kkrtc_video_capture_apis.h"
#include "kkrtc_std_wrapper.h"
#include <memory>
#include <string>
namespace kkrtc {
    namespace vcap {
        class ICapBackend {
        public:
            virtual ~ICapBackend(){}
            virtual std::shared_ptr<IVideoCapture> createCapture(int camera, const KKMediaFormat& params) const = 0;
            virtual std::shared_ptr<IVideoCapture> createCapture(const std::string &filename, const KKMediaFormat& params) const = 0;
        };

        class ICapBackendFactory {
        public:
            virtual ~ICapBackendFactory() {}
            virtual KKPtr<ICapBackend> getBackend() const = 0;
            virtual bool isBuiltIn() const = 0;
        };

        typedef std::shared_ptr<IVideoCapture> (*FN_createCaptureFile)(const std::string & filename);
        typedef std::shared_ptr<IVideoCapture> (*FN_createCaptureCamera)(int camera);
        typedef std::shared_ptr<IVideoCapture> (*FN_createCaptureFileWithParams)(const std::string & filename, const KKMediaFormat& params);
        typedef std::shared_ptr<IVideoCapture> (*FN_createCaptureCameraWithParams)(int camera, const KKMediaFormat& params);

        KKPtr<ICapBackendFactory> createStaticBackendFactory(FN_createCaptureFileWithParams createCaptureFile,
                                                  FN_createCaptureCameraWithParams createCaptureCamera);
        KKPtr<ICapBackendFactory> createPluginBackendFactory(KKVideoCaptureAPIs id, const char *baseName);

    }//vcap
}//kkrtc

#endif //KKRTC_KKRTC_CAP_BACKEND_INTERFACE_H
