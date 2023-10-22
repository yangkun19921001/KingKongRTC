//
// Created by devyk on 2023/10/22.
//

#ifndef KKRTC_VIDEO_CODEC_PLUGIN_APPI_H
#define KKRTC_VIDEO_CODEC_PLUGIN_APPI_H


#define VCODEC_API_VERSION 1
#define VCODEC_ABI_VERSION 1
#ifdef __cplusplus
extern "C" {
#endif
typedef struct KKVideoCodec_t *KkPluginVideoCodec;

typedef struct KkVideoCodecAPI {
    int vcodec_id;
    /**
    *  初始化
    *  @param
    *  handle：采集句柄
    *  param:全局参数
    * */
    int (*initialize)(KkPluginVideoCodec *handle, void *params);

    /**
     *  添加一个设置VideoFrame的指针,用于传递编码前的数据
     *   注意：我们使用了void*作为参数，这是因为C语言不支持C++模板。但在实际使用时，我们会传递一个VideoFrame的指针。
     * @param handle 视频处理句柄
     * @return
     */
    int (*send_frame)(KkPluginVideoCodec handle, void *video_frame);
    /**
     *  添加一个设置VideoPacket的指针,用于发送解码前的数据包
     *   注意：我们使用了void*作为参数，这是因为C语言不支持C++模板。但在实际使用时，我们会传递一个VideoPacket的指针。
     * @param handle 视频处理句柄
     * @return
     */
    int (*send_packet)(KkPluginVideoCodec handle, void *video_packet);
    /**
     * 设置编解码数据回调
     * @param handle
     * @param video_callback
     * @return
     */
    int (*set_video_callback)(KkPluginVideoCodec handle, void *video_callback);

    /**
     * 设置log回调
     * @param handle
     * @param video_log_callback
     * @return
     */
    int (*set_log_callback)(KkPluginVideoCodec handle, void *video_log_callback);

    /**
     *  取出编解码器缓冲区的数据
     *  @param
     *  handle：采集句柄
     *  camera_index:摄像头索引
     * */
    int (*flush)(KkPluginVideoCodec handle);

    /**
     *  释放编解码器
     *  @param
     *  handle：采集句柄
     *  camera_index:摄像头索引
     * */
    int (*release)(KkPluginVideoCodec handle);

} KkVideoCodecPluginAPI;

typedef const KkVideoCodecPluginAPI *(*FN_kkrtc_video_codec_plugin_init_t)
        (int requested_abi_version // 二进制版本
                , int requested_api_version // api 版本
                , void *reserved /*NULL*/);
}

#endif //KKRTC_VIDEO_CODEC_PLUGIN_APPI_H
