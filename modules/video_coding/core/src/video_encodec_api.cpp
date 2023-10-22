//
// Created by devyk on 2023/10/22.
//

#include "video_encodec_api.h"
#include "video_codec_registry.h"
#include "kkrtc_plugin_params.h"

kkrtc::vcodec::VideoEncoder::VideoEncoder() {

}

kkrtc::vcodec::VideoEncoder::~VideoEncoder() {

}

void kkrtc::vcodec::VideoEncoder::Initialize(kkrtc::KKMediaFormat &mediaFormat) {
    if (video_codec_.empty())return;
    video_codec_->Initialize(mediaFormat);
}

void kkrtc::vcodec::VideoEncoder::OnFrame(const kkrtc::VideoFrame &frame) {
    if (video_codec_.empty())return;
    video_codec_->SendFrame(frame);
}

void kkrtc::vcodec::VideoEncoder::OnEncodedPacket(const kkrtc::VideoPacket &packet) {
    if (encoder_callback_)encoder_callback_->OnEncodedPacket(packet);
}

int kkrtc::vcodec::VideoEncoder::RegisterVideoEncodeCompleteObserver(kkrtc::codec_core::VideoEncodedObserver *observer) {
    encoder_callback_ = observer;
    return 0;
}

int kkrtc::vcodec::VideoEncoder::RemoveVideoEncodeCompleteObserver() {
    encoder_callback_ = nullptr;
    return 0;
}

bool kkrtc::vcodec::VideoEncoder::Release() {
    if (!video_codec_.empty()) video_codec_.release();
    if (!plugin_glob_params_.empty()) plugin_glob_params_.release();
    return true;
}

bool kkrtc::vcodec::VideoEncoder::IsStarted() {
    if (video_codec_.empty()) return false;
    return video_codec_->IsStarted();
}

bool kkrtc::vcodec::VideoEncoder::LoadPlugin(const codec_core::KKVideoCodecAPIs apiPreference) {
    return LoadPlugin(apiPreference, kkrtc::KKMediaFormat());
}

bool kkrtc::vcodec::VideoEncoder::LoadPlugin(const  codec_core::KKVideoCodecAPIs apiPreference, const KKMediaFormat &format) {
    if (IsStarted()) {
        Release();
    }
    const std::vector<codec_core::VideoCodecBackendInfo> backends = kkrtc::vcodec_registry::getAvailableVideoCodecBackends();
    for (int i = 0; i < backends.size(); ++i) {
        const codec_core::VideoCodecBackendInfo &info = backends[i];
        if (apiPreference == codec_core::KK_VCODEC_ANY || apiPreference == info.id) {
            if (!info.vcodecBackendFactory) {
                KKLogDebugTag("VideoCodec") << "VIDEO  Codec(" << info.name
                                                << "): factory is not available (plugins require filesystem support)";
                continue;
            }
            KKLogDebugTag("VideoCodec") << info.name << " " << (int) apiPreference;
            assert(!info.vcodecBackendFactory.empty());
            const auto backend = info.vcodecBackendFactory->getBackend();
            if (!backend.empty()) {
                try {
                    plugin_glob_params_ = makePtr<kkrtc::PluginGlobParam_t>();
                    plugin_glob_params_->glob_logger = GetGlobLogger();
                    video_codec_ = backend->CreateVideoEncodec(format, this, this,plugin_glob_params_.get());
                    if (!video_codec_.empty()) {
                        if (video_codec_->IsStarted()) {
                            return true;
                        } else {
                            KKLogErrorTag("VideoCodec") << "VIDEO  Codec(" << info.name
                                                            << "):  load api:" << (int) apiPreference;
                        }
                    } else {
                        KKLogErrorTag("VideoCodec") << "VIDEO  Codec(" << info.name
                                                        << "): can't video  Codec";
                    }
                } catch (std::exception &e) {
                    KKLogErrorTag("VideoCodec") << "VIDEO  Codec(" << info.name
                                                    << "): raised C++ exception:"
                                                    << e.what();
                } catch (...) {
                    KKLogErrorTag("VideoCodec") << "VIDEO  Codec(" << info.name
                                                    << "): raised unknown C++ exception";

                }
            }
        }
    }
    return false;
}
