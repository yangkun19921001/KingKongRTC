//
// Created by devyk on 2023/10/22.
//

#include "ffmpeg_enc.h"
#include "core/include/video_codec_plugin_apis.h"
#include "core/include/video_codec_plugin_api.h"
#include "core/include/video_codec_interface.h"
#include "kkrtc_plugin_params.h"


kkrtc::vcodec_plugin::FFmpegVideoEncoder::FFmpegVideoEncoder() {

}

kkrtc::vcodec_plugin::FFmpegVideoEncoder::~FFmpegVideoEncoder() {

}

void kkrtc::vcodec_plugin::FFmpegVideoEncoder::Initialize(const kkrtc::KKMediaFormat &mediaFormats) {
    mediaformt_ = mediaFormats;
}

int kkrtc::vcodec_plugin::FFmpegVideoEncoder::SendFrame(const kkrtc::VideoFrame &videoframe) {
    return true;
}

int kkrtc::vcodec_plugin::FFmpegVideoEncoder::SendPacket(const kkrtc::VideoPacket &videoPacket) {
    return true;
}

bool kkrtc::vcodec_plugin::FFmpegVideoEncoder::IsStarted() const {
    return false;
}

int kkrtc::vcodec_plugin::FFmpegVideoEncoder::GetDomain() const {
    return kkrtc::codec_core::KK_VCODEC_FFMPEG;
}

const kkrtc::KKMediaFormat &kkrtc::vcodec_plugin::FFmpegVideoEncoder::GetMediaFormats() const {
    return mediaformt_;
}

void kkrtc::vcodec_plugin::FFmpegVideoEncoder::SetLogCallback(kkrtc::KKLogObserver *pLogObserver) {
    log_callback_ = pLogObserver;
}

void kkrtc::vcodec_plugin::FFmpegVideoEncoder::SetVideoEncodedCallback(
        kkrtc::codec_core::VideoEncodedObserver *pVideoEncodedObserver) {
    encodec_callback_ = pVideoEncodedObserver;
}

int kkrtc::vcodec_plugin::FFmpegVideoEncoder::Flush() {
    return 0;
}

int kkrtc::vcodec_plugin::FFmpegVideoEncoder::Release() {
    return 0;
}

static int kkrtc_initialize(KkPluginVideoCodec *handle, void *params) {
    *handle = nullptr;
    kkrtc::vcodec_plugin::FFmpegVideoEncoder *fFmpegVideoEncoder = nullptr;
    fFmpegVideoEncoder = new kkrtc::vcodec_plugin::FFmpegVideoEncoder();
    if (fFmpegVideoEncoder) {
        *handle = reinterpret_cast<KkPluginVideoCodec>(fFmpegVideoEncoder);
        if (params) {
            auto plugParam = static_cast<kkrtc::PluginGlobParam_t *>(params);
            if (plugParam->glob_logger) {
/*                auto globalLogger_ = static_cast<kkrtc::utils::log::ILogger *>(plugParam->glob_logger);
                auto logger = kkrtc::utils::log::LoggerFactory::createLogger(kkrtc::utils::log::LoggerId::SPDLOG,
                                                                             "DSHOW");
                logger->setLogLevel(globalLogger_->logLevel_);
                logger->setFileOutput(globalLogger_->fileOutputEnabled_, globalLogger_->filepath_);
                logger->setConsoleOutput(globalLogger_->consoleOutputEnabled_);
                //KKLogConfig_InitPluginLogger(static_cast<kkrtc::utils::log::ILogger*>(plugParam->glob_logger))
                KKLogConfig_InitGlobLogger(logger)
                KKLogInfoTag("DSHOW") << "initialize logger succeed.";*/
            }
            kkrtc::KKMediaFormat parameters(plugParam->c_params, plugParam->n_params);
            fFmpegVideoEncoder->Initialize(parameters);
        }
        return 0;
    }
    return -1;
}

static int kkrtc_video_callback(KkPluginVideoCodec handle, void *video_callback) {
    if (!handle)return -1;
    auto callback = static_cast<kkrtc::codec_core::VideoEncodedObserver *>(video_callback);
    auto vp = (kkrtc::vcodec_plugin::FFmpegVideoEncoder *) handle;
    vp->SetVideoEncodedCallback(callback);
    return callback == nullptr ? -1 : 0;
}

static int kkrtc_log_callback(KkPluginVideoCodec handle, void *log_callback) {
    if (!handle)return -1;
    auto callback = static_cast<kkrtc::KKLogObserver *>(log_callback);
    auto vp = (kkrtc::vcodec_plugin::FFmpegVideoEncoder *) handle;
    vp->SetLogCallback(callback);
    return callback == nullptr ? -1 : 0;
}

static int kkrtc_send_frame(KkPluginVideoCodec handle, void *video_frame) {
    if (!handle)return -1;
    auto vp = (kkrtc::vcodec_plugin::FFmpegVideoEncoder *) handle;
    auto videoFrame = (kkrtc::VideoFrame *) video_frame;
    vp->SendFrame(*videoFrame);
    return 0;
}

static int kkrtc_flush(KkPluginVideoCodec handle) {
    if (!handle)return -1;
    auto vp = (kkrtc::vcodec_plugin::FFmpegVideoEncoder *) handle;
    return vp->Flush();
}

static int kkrtc_release(KkPluginVideoCodec handle) {
    if (!handle)return -1;
    auto vp = (kkrtc::vcodec_plugin::FFmpegVideoEncoder *) handle;
    return vp->Release();
}

static const KkVideoCodecPluginAPI plugin_api =
        {
                .vcodec_id = kkrtc::codec_core::KK_VCODEC_FFMPEG,
                .initialize = kkrtc_initialize,
                .send_frame = kkrtc_send_frame,
                .send_packet = NULL,
                .set_video_callback = kkrtc_video_callback,
                .set_log_callback = kkrtc_log_callback,
                .flush= kkrtc_flush,
                .release =kkrtc_release,
        };

extern "C" RTC_EXPORT const KkVideoCodecPluginAPI *
kkrtc_video_codec_plugin_init(int requested_abi_version, int requested_api_version, void * /*reserved=NULL*/) {
    if (requested_abi_version == VCODEC_ABI_VERSION && requested_api_version <= VCODEC_API_VERSION)
        return &plugin_api;
    return NULL;
}
