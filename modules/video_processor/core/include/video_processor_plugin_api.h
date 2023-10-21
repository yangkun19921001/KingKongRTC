//
// Created by devyk on 2023/10/21.
//

#ifndef KKRTC_VIDEO_PROCESSOR_PLUGIN_API_H
#define KKRTC_VIDEO_PROCESSOR_PLUGIN_API_H
#define VPRO_API_VERSION 1
#define VPRO_ABI_VERSION 1
#ifdef __cplusplus
extern "C" {
#endif
typedef struct KKVideoProcessor_t *KkPluginVideoProcessor;

typedef struct KkVideoProcessorAPI {
    int vpro_id;
    /**
    *  初始化
    *  @param
    *  handle：采集句柄
    *  param:全局参数
    * */
    int (*initialize)(KkPluginVideoProcessor *handle,void *params);
    /**
     *  添加一个设置VideoFrame的指针
     *   注意：我们使用了void*作为参数，这是因为C语言不支持C++模板。但在实际使用时，我们会传递一个VideoFrame的指针。
     * @param handle 视频处理句柄
     * @return
     */
    int (*processor)(KkPluginVideoProcessor handle, void *video_frame);

    int (*set_log_callback)(KkPluginVideoProcessor handle, void *video_log_callback);

    /**
     *  释放视频处理器
     *  @param
     *  handle：采集句柄
     *  camera_index:摄像头索引
     * */
    int (*release)(KkPluginVideoProcessor handle);

} KkVideoProcessorPluginAPI;

typedef const KkVideoProcessorPluginAPI *(*FN_kkrtc_video_processor_plugin_init_t)
        (int requested_abi_version // 二进制版本
                , int requested_api_version // api 版本
                , void *reserved /*NULL*/);
}
#endif //KKRTC_VIDEO_PROCESSOR_PLUGIN_API_H
