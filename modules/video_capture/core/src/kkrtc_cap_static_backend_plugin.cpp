//
// Created by devyk on 2023/10/9.
//
#include "kkrtc_cap_backend_interface.h"

namespace kkrtc {
    namespace vcap {
        class StaticCapBackend : public ICapBackend {
        public:
            FN_createCaptureFileWithParams fn_createCaptureFile_;
            FN_createCaptureCameraWithParams fn_createCaptureCamera_;

            StaticCapBackend();
        };
    }//namespace vcap
}//namespace kkrtc