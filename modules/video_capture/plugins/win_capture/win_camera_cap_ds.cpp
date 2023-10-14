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

namespace kkrtc {
    namespace vcap {

        WinCameraCaptureDS::~WinCameraCaptureDS() {
            video_sink_interface_ = nullptr;
            Close();
            CoUninitialize();
        }

        WinCameraCaptureDS::WinCameraCaptureDS()
                : m_camera_index_(-1), m_width(-1), m_height(-1), m_framerate(-1),
                  m_fourcc((int)DShow::VideoFormat::I420), m_auto_focus(true),
                  video_sink_interface_(nullptr) {
            // 初始化 COM 库
            CoInitialize(0);
        }

        WinCameraCaptureDS::WinCameraCaptureDS(int index, KKMediaFormat &mediaFormat)
                : m_camera_index_(index), m_width(-1), m_height(-1), m_framerate(-1), m_fourcc((int)DShow::VideoFormat::I420),
                  media_format_(mediaFormat),
                  m_auto_focus(true),
                  video_sink_interface_(nullptr) {
            // 初始化 COM 库
            CoInitialize(0);
        }

        static void logcallback(DShow::LogType type, const wchar_t *libmsg, void *param){
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string narrowStr = converter.to_bytes(libmsg);
            const char * msg = narrowStr.c_str();
            switch (type) {
                case DShow::LogType::Error:
                    KKLogErrorTag("LIBDSHOW") << msg;
                    break;
                case DShow::LogType::Warning:
                    KKLogWarnTag("LIBDSHOW") << msg;
                    break;
                case DShow::LogType::Debug:
                    KKLogDebugTag("LIBDSHOW") << msg;
                    break;
                case DShow::LogType::Info:
                    KKLogInfoTag("LIBDSHOW") << msg;
                    break;
            }
        }
        void WinCameraCaptureDS::Initialize(const kkrtc::KKMediaFormat &mediaFormats) {
            media_format_ = mediaFormats;
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
                                m_fourcc = (int)DShow::VideoFormat::I420;
                                break;
                            case RGB24:
                                m_fourcc = (int)DShow::VideoFormat::RGB24;
                                break;
                        }
                        break;
                    }
                    case KK_CAP_PROP_AUTOFOCUS:
                        m_auto_focus = mediaFormats.get<bool>(KK_CAP_PROP_AUTOFOCUS);
                        break;
                    default: {
                        KKLogWarnTag("WinCameraCaptureDS") << "format not used:" << mediaFormats.getIntVector()[i];
                        break;
                    }
                }
            }
            DShow::SetLogCallback(logcallback,this);
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
                                     long long endTime, long rotation){
            std::cout << " OnVideoData " << config.cx<<":"<<config.cy_abs << (int)config.format<< std::endl;

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
                std::cout << " ResetGraph error " << std::endl;
                KKLogError << " DSHOW ResetGraph error ";
                return false;
            }
            std::vector<DShow::VideoDevice> devices;
            DShow::Device::EnumVideoDevices(devices);
            videoConfig_.format = (DShow::VideoFormat)m_fourcc;
            if (m_framerate > 0) videoConfig_.frameInterval = m_framerate;
            videoConfig_.internalFormat = (DShow::VideoFormat)m_fourcc;
            videoConfig_.callback = std::bind(&WinCameraCaptureDS::OnVideoData, this,
                                             std::placeholders::_1, std::placeholders::_2,
                                             std::placeholders::_3, std::placeholders::_4,
                                             std::placeholders::_5, std::placeholders::_6);
            if (devices.size() == 0 || devices.size() < m_camera_index_ +1)
            {
                KKLogError << "devices size:"<<devices.size();
                return false;
            }
            videoConfig_.name = devices[m_camera_index_].name;
            videoConfig_.path = devices[m_camera_index_].path;
            if (m_width > 0) videoConfig_.cx = m_width;
            if (m_height > 0) videoConfig_.cy_abs = m_height;
            videoConfig_.useDefaultConfig = true;
            device_.SetVideoConfig(&videoConfig_);
            if (!device_.ConnectFilters()){
                KKLogErrorTag("WIN VIDEO DSHOW") <<  " ConnectFilters error";
                return false;
            }


            if (device_.Start() != DShow::Result::Success)
            {
                KKLogErrorTag("WIN VIDEO DSHOW") <<  " Start error";
                return false;
            }
            m_camera_index_ = index;
            return 0;
        }

        void WinCameraCaptureDS::SetVideoSink(kkrtc::VideoSinkInterface<VideoFrame> *videoSinkInterface) {
            video_sink_interface_ = videoSinkInterface;
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
            KKLogWarn << "WIN_DSHOW Open Error:" << camera_index;
        }
        if (win_cap_ds->IsOpened()) {
            win_cap_ds->GetCapDomain();
            return 0;
        }
        return -1;
    } catch (const std::exception &e) {
        KKLogErrorTag("Windows DS Capture") << "DirectShow: Exception is raised: " << e.what();

    } catch (...) {
        KKLogErrorTag("Windows DS Capture") << "DirectShow: Unknown C++ exception is raised ";
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
void kkrtc_set_video_sink(KkPluginCapture handle, void *video_sink_interface) {
    if (!handle)return;
    auto sink = static_cast<kkrtc::VideoSinkInterface<kkrtc::VideoFrame> *>(video_sink_interface);
    auto win_cap_ds = (kkrtc::vcap::WinCameraCaptureDS *) handle;
    win_cap_ds->SetVideoSink(sink);
}

static const KKVideoCapturePluginAPI capture_plugin_api =
        {
                .cap_id = kkrtc::vcap::KK_CAP_DSHOW,
                .capture_initialize = kkrtc_capture_initialize,
                .capture_open_with_params = kkrtc_capture_open_with_params,
                .set_video_sink = kkrtc_set_video_sink,
                .capture_switch =kkrtc_switch_camera,
                .capture_release =kkrtc_release_camera,
        };

extern "C" RTC_EXPORT const KKVideoCapturePluginAPI *
kkrtc_video_capture_plugin_init(int requested_abi_version, int requested_api_version, void * /*reserved=NULL*/) {
    if (requested_abi_version == CAPTURE_ABI_VERSION && requested_api_version <= CAPTURE_API_VERSION)
        return &capture_plugin_api;
    return NULL;
}