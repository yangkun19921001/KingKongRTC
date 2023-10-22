//
// Created by devyk on 2023/10/21.
//

#include "default_video_processor.h"
#include "kkrtc_plugin_params.h"

namespace kkrtc {
    namespace plugin {
        namespace def {
            DefaultVideoPorcessor::~DefaultVideoPorcessor() {

            }

            DefaultVideoPorcessor::DefaultVideoPorcessor() {

            }

            bool DefaultVideoPorcessor::IsStarted() const {
                //return this->frames_ > 0;
                return true;
            }

            bool DefaultVideoPorcessor::Process(VideoFrame &videoFrame) {
                ++this->frames_;
                return video_processor_chain_.process(videoFrame);
            }

            const KKMediaFormat &DefaultVideoPorcessor::GetMediaFormats() const {
                return media_format_;
            }

            void DefaultVideoPorcessor::Initialize(const KKMediaFormat &mediaFormats) {
                this->media_format_ = mediaFormats;
                //todo ... 美颜->滤镜->等
//                video_processor_chain_.addProcessor()
            }

            void DefaultVideoPorcessor::SetLogCallback(KKLogObserver *pObserver) {
                this->log_callback_ = pObserver;
            }
        } //def
    }//plugins
}//kkrtc


static int kkrtc_initialize(KkPluginVideoProcessor *handle, void *param) {
    *handle = nullptr;
    kkrtc::plugin::def::DefaultVideoPorcessor *def = nullptr;
    def = new kkrtc::plugin::def::DefaultVideoPorcessor();
    if (def) {
        *handle = (KkPluginVideoProcessor) def;
        if (param) {
            auto plugParam = static_cast<kkrtc::PluginGlobParam_t *>(param);
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
            def->Initialize(parameters);
        }
        return 0;
    }
    return -1;
}


static
int kkrtc_set_log_callback(KkPluginVideoProcessor handle, void *log_callback) {
    if (!handle)return -1;
    auto callback = static_cast<kkrtc::KKLogObserver *>(log_callback);
    auto vp = (kkrtc::plugin::def::DefaultVideoPorcessor *) handle;
    vp->SetLogCallback(callback);
    return callback == nullptr ? -1 : 0;
}

static int kkrtc_processor(KkPluginVideoProcessor handle, void *video_frame) {
    if (!handle || !video_frame)return -1;
    auto vp = (kkrtc::plugin::def::DefaultVideoPorcessor *) handle;
    auto videoFrame = (kkrtc::VideoFrame *) video_frame;
    return vp->Process(*videoFrame) ? 0 : -1;

}

static int kkrtc_release(KkPluginVideoProcessor handle) {
    if (!handle)return -1;
    auto vp = (kkrtc::plugin::def::DefaultVideoPorcessor *) handle;
    delete vp;
    return 0;

}

static const KkVideoProcessorPluginAPI plugin_api =
        {
                .vpro_id = kkrtc::vpro::KK_VPRO_DEF,
                .initialize = kkrtc_initialize,
                .processor = kkrtc_processor,
                .set_log_callback = kkrtc_set_log_callback,
                .release =kkrtc_release,
        };

extern "C" RTC_EXPORT const KkVideoProcessorPluginAPI *
kkrtc_video_processor_plugin_init(int requested_abi_version, int requested_api_version, void * /*reserved=NULL*/) {
    if (requested_abi_version == VPRO_ABI_VERSION && requested_api_version <= VPRO_API_VERSION)
        return &plugin_api;
    return NULL;
}