//
// Created by devyk on 2023/9/15.
//

#ifndef KKRTC_KKRTC_CAP_PLUGIN_API_H
#define KKRTC_KKRTC_CAP_PLUGIN_API_H

#include "kkrtc_plugin_params.h"

#if !defined(BUILD_PLUGIN)

/// increased for backward-compatible changes, e.g. add new function
/// Caller API <= Plugin API -> plugins is fully compatible
/// Caller API > Plugin API -> plugins is not fully compatible, caller should use extra checks to use plugins with older API
#define CAPTURE_API_VERSION 1

/// increased for incompatible changes, e.g. remove function argument
/// Caller ABI == Plugin ABI -> plugins is compatible
/// Caller ABI > Plugin ABI -> plugins is not compatible, caller should use shim code to use old ABI plugins (caller may know how lower ABI works, so it is possible)
/// Caller ABI < Plugin ABI -> plugins can't be used (plugins should provide interface with lower ABI to handle that)
#define CAPTURE_ABI_VERSION 1

#else // !defined(BUILD_PLUGIN)

#if !defined(CAPTURE_ABI_VERSION) || !defined(CAPTURE_API_VERSION)
#error "Plugin must define CAPTURE_ABI_VERSION and CAPTURE_API_VERSION before including plugin_capture_api.hpp"
#endif

#endif // !defined(BUILD_PLUGIN)



#ifdef __cplusplus
extern "C" {
#endif
typedef struct KKVideoCapture_t *KkPluginCapture;


typedef struct KKVideoCaptureAPI {
    int cap_id;

    /**
    *  初始化
    *  @param
    *  handle：采集句柄
    *  param:全局参数
    * */
    int (*capture_initialize)(KkPluginCapture *handle,void *param);

    /**
    *  打开摄像头
    *  @param
    *  handle：采集句柄
    *  camera_index:摄像头索引
    * */
    int (*capture_open_with_params)(KkPluginCapture handle, int camera_index, const char *filename, int *params,
                                    unsigned n_params);

    /**
     *  添加一个设置VideoCaptureObserver的指针
     *   注意：我们使用了void*作为参数，这是因为C语言不支持C++模板。但在实际使用时，我们会传递一个VideoCaptureObserver的指针。
     * @param handle 采集句柄
     * @param video_sink_interface 视频数据的回调函数
     * @return
     */
    int (*set_video_callback)(KkPluginCapture handle, void *video_cap_callback);

    /**
     *  添加一个设置VideoCaptureObserver的指针
     *   注意：我们使用了void*作为参数，这是因为C语言不支持C++模板。但在实际使用时，我们会传递一个VideoCaptureObserver的指针。
     * @param handle 采集句柄
     * @param video_sink_interface 视频数据的回调函数
     * @return
     */
    int (*set_log_callback)(KkPluginCapture handle, void *video_log_callback);

    /**
     *  切换摄像头
     *  @param
     *  handle：采集句柄
     *  camera_index:摄像头索引
     * */
    int (*capture_switch)(KkPluginCapture handle, int camera_index);

    /**
     *  释放摄像头
     *  @param
     *  handle：采集句柄
     *  camera_index:摄像头索引
     * */
    int (*capture_release)(KkPluginCapture handle);

} KKVideoCapturePluginAPI;

/*const KKVideoCapturePluginAPI*  FN_kkrtc_video_capture_plugin_init_t
        (int requested_abi_version, int requested_api_version, void* reserved *//*NULL*//*)*/

typedef const KKVideoCapturePluginAPI *(*FN_kkrtc_video_capture_plugin_init_t)
        (int requested_abi_version // 二进制版本
                , int requested_api_version // api 版本
                , void *reserved /*NULL*/);
}
#endif //KKRTC_KKRTC_CAP_PLUGIN_API_H
