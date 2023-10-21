//
// Created by devyk on 2023/10/21.
//

#include "video_processor_api.h"
#include "video_processor_registry.h"

namespace kkrtc {
    namespace vpro {
            VideoProcessor::~VideoProcessor() {

            }

            VideoProcessor::VideoProcessor() {

            }

            void VideoProcessor::Release() {
                video_processors_.clear();
            }

            bool VideoProcessor::IsStarted() {
                return video_processors_.size() > 0;
            }

            void VideoProcessor::OnFrame(const kkrtc::VideoFrame &frame) {
                kkrtc::VideoFrame mutableFrame = frame;
                for (auto &processor: video_processors_) {
                    processor->Process(mutableFrame);
                }
                video_broadcaster_.OnFrame(mutableFrame);
            }

            void VideoProcessor::AddSink(VideoSinkInterface<kkrtc::VideoFrame> *sink, const VideoSinkWants &wants) {
                video_broadcaster_.AddOrUpdateSink(sink, wants);
            }

            void VideoProcessor::RemoveSink(VideoSinkInterface<kkrtc::VideoFrame> *sink) {
                video_broadcaster_.RemoveSink(sink);
            }

            bool VideoProcessor::LoadPlugin(const KKVideoProcessorAPIs apiPreference) {
                return LoadPlugin(apiPreference,kkrtc::KKMediaFormat());
            }

            bool VideoProcessor::LoadPlugin(const KKVideoProcessorAPIs apiPreference, const KKMediaFormat &format) {
                if (IsStarted()) {
                    Release();
                }
                const std::vector<VideoProcessorBackendInfo> backends = kkrtc::vpro_registry::getAvailableVideoProcessorBackends();
                for (int i = 0; i < backends.size(); ++i) {
                    const VideoProcessorBackendInfo &info = backends[i];
                    if (apiPreference == KK_VPRO_ANY || apiPreference == info.id) {
                        if (!info.vproBackendFactory) {
                            KKLogDebugTag("VideoProcessor") << "VIDEO PROCESSOR(" << info.name
                                                            << "): factory is not available (plugins require filesystem support)";
                            continue;
                        }
                        KKLogDebugTag("VideoProcessor") << info.name << " " << (int) apiPreference;
                        assert(!info.vproBackendFactory.empty());
                        const auto backend = info.vproBackendFactory->getBackend();
                        if (!backend.empty()) {
                            try {
                                plugin_glob_params_ = makePtr<kkrtc::PluginGlobParam_t>();
                                plugin_glob_params_->glob_logger = GetGlobLogger();
                                auto vpro = backend->CreateVideoProcessor(format, this, plugin_glob_params_.get());
                                if (!vpro.empty()) {
                                    if (vpro->IsStarted()) {
                                        video_processors_.push_back(vpro);
                                        continue;
                                    } else {
                                        KKLogErrorTag("VideoProcessor") << "VIDEO PROCESSOR(" << info.name
                                                                        << "):  load api:" << (int) apiPreference;
                                    }
                                    vpro.release();
                                } else {
                                    KKLogErrorTag("VideoProcessor") << "VIDEO PROCESSOR(" << info.name
                                                                    << "): can't video processor";
                                }
                            } catch (std::exception &e) {
                                KKLogErrorTag("VideoProcessor") << "VIDEO PROCESSOR(" << info.name
                                                                << "): raised C++ exception:"
                                                                << e.what();
                            } catch (...) {
                                KKLogErrorTag("VideoProcessor") << "VIDEO PROCESSOR(" << info.name
                                                                << "): raised unknown C++ exception";

                            }
                        }
                    }
                }
                return video_processors_.size() > 0;
            }
    }//vpro
}//kkrtc