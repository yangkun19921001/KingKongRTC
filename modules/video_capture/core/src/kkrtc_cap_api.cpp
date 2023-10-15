//
// Created by devyk on 2023/10/8.
//

#include "kkrtc_cap_api.h"
#include "kkrtc_cap_registry.h"

namespace kkrtc {
    namespace vcap {

        VideoCapture::VideoCapture() {

        }

        VideoCapture::~VideoCapture() {

        }

        VideoCapture::VideoCapture(int index, int apiPreference, const KKMediaFormat &params) {
            open(index, apiPreference, params);
        }

        VideoCapture::VideoCapture(const String &filename, int apiPreference, const KKMediaFormat &params) {
            open(filename, apiPreference, params);
        }

        bool VideoCapture::open(const String &filename, int apiPreference, const KKMediaFormat &params) {
            return false;
        }

        bool VideoCapture::open(int index, int apiPreference, const KKMediaFormat &params) {
            if (isOpened()) {
                release();
            }
            const std::vector<VideoCapBackendInfo> backends = kkrtc::vcap_registry::getAvailableBackends_CaptureByIndex();
            for (int i = 0; i < backends.size(); ++i) {
                const VideoCapBackendInfo &info = backends[i];
                if (apiPreference == KK_CAP_ANY || apiPreference == info.id) {
                    if (!info.capBackendFactory)
                    {
                        KKLogDebugTag("kkrtc_cap_api")<< "VIDEOIO(" << info.name << "): factory is not available (plugins require filesystem support)";
                        continue;
                    }

                    KKLogDebugTag("kkrtc_cap_api")<<
                                         std::format("VIDEOIO(%s): trying capture cameraNum=%d ...",
                                                    info.name, index);
                    assert(!info.capBackendFactory.empty());
                    const KKPtr<ICapBackend> backend = info.capBackendFactory->getBackend();
                    if (!backend.empty())
                    {
                        try {
                            cap_ = backend->createCapture(index,params,this, this);
                            if (!cap_.empty())
                            {
                                if (cap_->IsOpened())
                                {
                                    return true;
                                } else{
                                    KKLogErrorTag("VideoCapture")<<"VIDEOIO("<< info.name <<"):  open error. index:"<<index;
                                }
                                cap_.release();
                            } else {

                                KKLogErrorTag("VideoCapture")<<"VIDEOIO("<< info.name <<"): can't create capture";

                            }

                        }catch (std::exception& e){
                            KKLogErrorTag("VideoCapture")<<"VIDEOIO("<< info.name <<"): raised C++ exception:"<<e.what();
                        }catch (...){
                            KKLogErrorTag("VideoCapture")<<"VIDEOIO("<< info.name <<"): raised unknown C++ exception";

                        }

                    }
                }
            }
            return false;
        }

        bool VideoCapture::isOpened() const {
            return !cap_.empty() && cap_->IsOpened();
        }

        void VideoCapture::release() {
            cap_.release();
        }

        void VideoCapture::IncomingFrame(uint8_t *videoFrame, size_t videoFrameLength, const KKVideoCapConfig &config,
                                         int64_t captureTime) {
            KKLogDebugTag("IncomingFrame") << config.width << ":" << config.height << " format:" << (int) config.videoType
                       <<"  fps:" << (int) config.maxFPS
                       <<"  startTime:" << captureTime;
        }
    }// namespace vcap
}// namespace kkrtc