//
// Created by devyk on 2023/9/15.
//

#include <objbase.h>
#include <wmcodecdsp.h>
#include "win_camera_cap_ds.h"
#include "kkrtc_cap_plugin_api.h"
#include "kkrtc_video_capture_apis.h"
#include "log_helper.h"
#include <functional>
#include <codecvt>
#include "kkrtc_plugin_log_observer.h"

const char * plugin_name = "PLUGIN-DSHOW";

namespace kkrtc {
    namespace vcap {
        WinCameraCaptureDS::~WinCameraCaptureDS() {
            video_callback_ = nullptr;
            Close();
            CoUninitialize();
        }

        WinCameraCaptureDS::WinCameraCaptureDS()
                : m_camera_index_(-1), m_width(-1), m_height(-1), m_framerate(-1),
                  m_fourcc(-1), m_auto_focus(true), log_callback_(nullptr),
                  video_callback_(nullptr) {
            // 初始化 COM 库
            CoInitialize(0);
        }

        WinCameraCaptureDS::WinCameraCaptureDS(int index, KKMediaFormat &mediaFormat)
                : m_camera_index_(index), m_width(-1), m_height(-1), m_framerate(-1), m_fourcc(-1),
                  media_format_(mediaFormat),
                  m_auto_focus(true),
                  video_callback_(nullptr) {
            // 初始化 COM 库
            CoInitialize(0);
        }

        static void logcallback(DShow::LogType type, const wchar_t *libmsg, void *param) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string narrowStr = converter.to_bytes(libmsg);
            const char *msg = narrowStr.c_str();
            auto callback = (kkrtc::KKLogObserver *) param;
            switch (type) {
                case DShow::LogType::Error:
                    PLUGIN_LOG_ERR(plugin_name,msg, callback);
                    break;
                case DShow::LogType::Warning:
                    PLUGIN_LOG_WARN(plugin_name,msg, callback);
                    break;
                case DShow::LogType::Debug:
                    PLUGIN_LOG_DEBUG(plugin_name,msg, callback);
                    break;
                case DShow::LogType::Info:
                    PLUGIN_LOG_INFO(plugin_name,msg, callback);
                    break;
            }
        }

        void WinCameraCaptureDS::Initialize(const kkrtc::KKMediaFormat &mediaFormats) {
            media_format_ = mediaFormats;
            DShow::SetLogCallback(logcallback, this->log_callback_);
            auto intv = mediaFormats.getIntVector();
            for (int i = 0; i < mediaFormats.getIntVector().size(); i += 2) {
                switch (mediaFormats.getIntVector()[i]) {
                    case KK_CAP_PROP_FPS:
                        m_framerate = mediaFormats.get<int>(KK_CAP_PROP_FPS);
                        break;
                    case KK_CAP_PROP_FRAME_WIDTH:
                        m_width = mediaFormats.get<int>(KK_CAP_PROP_FRAME_WIDTH);
                        break;
                    case KK_CAP_PROP_FRAME_HEIGHT:
                        m_height = mediaFormats.get<int>(KK_CAP_PROP_FRAME_HEIGHT);
                        break;
                    case KK_CAP_PROP_COLOR_FORMAT: {
                        switch (mediaFormats.get<int>(KK_CAP_PROP_COLOR_FORMAT)) {
                            case I420:
                                m_fourcc = (int) DShow::VideoFormat::I420;
                                break;
                            case RGB24:
                                m_fourcc = (int) DShow::VideoFormat::RGB24;
                                break;
                        }
                        break;
                    }
                    case KK_CAP_PROP_AUTOFOCUS:
                        m_auto_focus = mediaFormats.get<bool>(KK_CAP_PROP_AUTOFOCUS);
                        break;
                    default: {
                        PLUGIN_LOG_WARN(plugin_name,std::format("format not used:{}", mediaFormats.getIntVector()[i]).c_str(),
                                        this->log_callback_);
                        break;
                    }
                }
            }
        }

        bool WinCameraCaptureDS::IsOpened() const {
            return (-1 != m_camera_index_);
        }

        int WinCameraCaptureDS::GetCapDomain() const {

            return KK_CAP_DSHOW;
        }

        const kkrtc::KKMediaFormat &WinCameraCaptureDS::GetMediaFormats() const {
            return media_format_;
        }

        void WinCameraCaptureDS::OnVideoData(const DShow::VideoConfig &config, unsigned char *data,
                                             size_t size, long long startTime,
                                             long long endTime, long rotation) {
/*            PLUGIN_LOG_DEBUG(plugin_name,
                    std::format("OnVideoData: {} x {} format:{} fps:{} startTime:{} endTime:{} rotation:{}",
                                config.cx, config.cy_abs, (int) config.format, (int) config.frameInterval, startTime,
                                endTime, rotation).c_str(), this->log_callback_);*/
            if (this->video_callback_) {
                KKVideoCapConfig kk_config;
                kk_config.rotation = rotation;
                kk_config.height = config.cy_abs;
                kk_config.width = config.cx;
                if (config.frameInterval > 0) kk_config.maxFPS = 1000.0 / (config.frameInterval / 10000.0);
                switch (config.format) {
                    case DShow::VideoFormat::I420:
                        kk_config.videoType = KKVideoFormat::I420;
                        break;
                    case DShow::VideoFormat::RGB24:
                        kk_config.videoType = KKVideoFormat::RGB24;
                        break;
                    case DShow::VideoFormat::ARGB:
                        kk_config.videoType = KKVideoFormat::ARGB;
                        break;
                    case DShow::VideoFormat::YUY2:
                        kk_config.videoType = KKVideoFormat::YUY2;
                        break;
                    case DShow::VideoFormat::YV12:
                        kk_config.videoType = KKVideoFormat::YV12;
                        break;
                    case DShow::VideoFormat::YVYU:
                        kk_config.videoType = KKVideoFormat::YVYU;
                        break;
                    case DShow::VideoFormat::NV12:
                        kk_config.videoType = KKVideoFormat::NV12;
                        break;
                    case DShow::VideoFormat::MJPEG:
                        kk_config.videoType = KKVideoFormat::MJPEG;
                        break;
                }
                this->video_callback_->IncomingFrame(data, size, kk_config, startTime);
            }
        }

        void WinCameraCaptureDS::Close() {
            if (m_camera_index_ >= 0 && device_.Valid()) {
                device_.Stop();
                DShow::SetLogCallback(nullptr, nullptr);
                m_camera_index_ = -1;
                m_width = m_height = -1;
            }
        }

        int WinCameraCaptureDS::Open(int index) {
            Close();
            if (!device_.ResetGraph()) {
                PLUGIN_LOG_ERR(plugin_name,std::format("ResetGraph error"
                ).c_str(), this->log_callback_)
                return false;
            }
            std::vector<DShow::VideoDevice> devices;
            DShow::Device::EnumVideoDevices(devices);

            if (devices.size() == 0 || devices.size() < m_camera_index_ + 1) {
                PLUGIN_LOG_ERR(plugin_name,std::format("devices size:{}", devices.size()
                ).c_str(), this->log_callback_)
                return false;
            }

            if (m_framerate > 0) videoConfig_.frameInterval = m_framerate;
            videoConfig_.name = devices[m_camera_index_].name;
            videoConfig_.path = devices[m_camera_index_].path;
            if (m_fourcc >= 100) {
                videoConfig_.internalFormat = (DShow::VideoFormat) m_fourcc;
                videoConfig_.format = (DShow::VideoFormat) m_fourcc;
            } else {
                videoConfig_.internalFormat = devices[m_camera_index_].caps[0].format;
                videoConfig_.format = videoConfig_.internalFormat;
            }
            if (m_width > 0)
                videoConfig_.cx = m_width;
            else {
                m_height = devices[m_camera_index_].caps[0].maxCX;
            }
            if (m_height > 0) videoConfig_.cy_abs = m_height;
            else {
                m_height = devices[m_camera_index_].caps[0].maxCY;
            }
            videoConfig_.useDefaultConfig = false;
            videoConfig_.callback = std::bind(&WinCameraCaptureDS::OnVideoData, this,
                                              std::placeholders::_1, std::placeholders::_2,
                                              std::placeholders::_3, std::placeholders::_4,
                                              std::placeholders::_5, std::placeholders::_6);
            device_.SetVideoConfig(&videoConfig_);
            if (!device_.ConnectFilters()) {
                PLUGIN_LOG_ERR(plugin_name,std::format("ConnectFilters error"
                ).c_str(), this->log_callback_)
                return false;
            }


            if (device_.Start() != DShow::Result::Success) {
                PLUGIN_LOG_ERR(plugin_name,std::format("Start error"
                ).c_str(), this->log_callback_)
                return false;
            }
            m_camera_index_ = index;
            return 0;
        }

        void WinCameraCaptureDS::SetVideoCallback(VideoCaptureObserver *videoCaptureObserver) {
            this->video_callback_ = videoCaptureObserver;
        }

        void WinCameraCaptureDS::SetLogCallback(kkrtc::KKLogObserver *kk_log_callback) {
            this->log_callback_ = kk_log_callback;
        }
    }//vcap
}//kkrtc


static int kkrtc_capture_initialize(KkPluginCapture *handle) {
    *handle = nullptr;
    kkrtc::vcap::WinCameraCaptureDS *win_cap_ds = nullptr;
    win_cap_ds = new kkrtc::vcap::WinCameraCaptureDS();
    if (win_cap_ds) {
        *handle = (KkPluginCapture) win_cap_ds;
        return 0;
    }
    return -1;
}

static
int kkrtc_capture_open_with_params(KkPluginCapture handle, int camera_index, const char *filename, int *params,
                                   unsigned n_params) {
    if (!handle)return -1;
    auto win_cap_ds = (kkrtc::vcap::WinCameraCaptureDS *) handle;
    try {
        kkrtc::KKMediaFormat parameters(params, n_params);
        win_cap_ds->m_camera_index_ = camera_index;
        win_cap_ds->Initialize(parameters);
        int ret = win_cap_ds->Open(camera_index);
        if (ret != 0) {
            PLUGIN_LOG_ERR(plugin_name,std::format("WIN_DSHOW Open Error:{}", camera_index
            ).c_str(), win_cap_ds->log_callback_)
        }
        if (win_cap_ds->IsOpened()) {
            win_cap_ds->GetCapDomain();
            return 0;
        }
        return -1;
    } catch (const std::exception &e) {
        PLUGIN_LOG_ERR(plugin_name,std::format("DirectShow: Exception is raised:{}", e.what()
        ).c_str(), win_cap_ds->log_callback_)
    } catch (...) {
        PLUGIN_LOG_ERR(plugin_name,std::format("DirectShow: Unknown C++ exception is raised"
        ).c_str(), win_cap_ds->log_callback_)
    }

    return -1;
}

static
int kkrtc_switch_camera(KkPluginCapture handle, int camera_index) {
    if (!handle)return -1;
    auto win_cap_ds = (kkrtc::vcap::WinCameraCaptureDS *) handle;

    return win_cap_ds->Open(camera_index);
}

static
int kkrtc_release_camera(KkPluginCapture handle) {
    if (!handle)return -1;
    auto win_cap_ds = (kkrtc::vcap::WinCameraCaptureDS *) handle;
    delete win_cap_ds;
    return -1;
}

static
int kkrtc_set_video_callback(KkPluginCapture handle, void *video_callback) {
    if (!handle)return -1;
    auto callback = static_cast<kkrtc::vcap::VideoCaptureObserver *>(video_callback);
    auto win_cap_ds = (kkrtc::vcap::WinCameraCaptureDS *) handle;
    win_cap_ds->SetVideoCallback(callback);
    return callback == nullptr ? -1 : 0;
}

static
int kkrtc_set_log_callback(KkPluginCapture handle, void *log_callback) {
    if (!handle)return -1;
    auto callback = static_cast<kkrtc::KKLogObserver *>(log_callback);
    auto win_cap_ds = (kkrtc::vcap::WinCameraCaptureDS *) handle;
    win_cap_ds->SetLogCallback(callback);
    return callback == nullptr ? -1 : 0;
}

static const KKVideoCapturePluginAPI capture_plugin_api =
        {
                .cap_id = kkrtc::vcap::KK_CAP_DSHOW,
                .capture_initialize = kkrtc_capture_initialize,
                .capture_open_with_params = kkrtc_capture_open_with_params,
                .set_video_callback = kkrtc_set_video_callback,
                .set_log_callback = kkrtc_set_log_callback,
                .capture_switch =kkrtc_switch_camera,
                .capture_release =kkrtc_release_camera,
        };

extern "C" RTC_EXPORT const KKVideoCapturePluginAPI *
kkrtc_video_capture_plugin_init(int requested_abi_version, int requested_api_version, void * /*reserved=NULL*/) {
    if (requested_abi_version == CAPTURE_ABI_VERSION && requested_api_version <= CAPTURE_API_VERSION)
        return &capture_plugin_api;
    return NULL;
}