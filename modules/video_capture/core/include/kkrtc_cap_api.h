//
// Created by devyk on 2023/10/8.
//

#ifndef KKRTC_KKRTC_CAP_API_H
#define KKRTC_KKRTC_CAP_API_H

#include <vector>
#include "kkrtc_std.h"
#include "kkrtc_cap_interface.h"
#include "kkrtc_plugin_log_observer.h"
#include "kkrtc_plugin_params.h"
namespace kkrtc {
    namespace vcap {
        /**
        * @brief VideoCapture 类提供一个用于捕获视频数据的接口。
        * 它支持使用各种API从文件和设备打开视频。
        */
        class VideoCapture : public VideoCaptureObserver, kkrtc::KKLogObserver, VideoCaptureModule {
        public:
            /**
             * @brief VideoCapture 的默认构造函数。
             */
            VideoCapture();

            /**
             * @brief 用指定的设备索引、API偏好和参数构造一个 VideoCapture 对象。
             *
             * @param index 要捕获的设备索引。
             * @param apiPreference 优选的视频捕获API。
             * @param params 打开设备时的参数列表。
             */
            explicit VideoCapture(int index, int apiPreference, const KKMediaFormat &params);

            /**
             * @brief 用指定的文件名、API偏好和参数构造一个 VideoCapture 对象。
             *
             * @param filename 要打开的视频文件名。
             * @param apiPreference 优选的视频捕获API。
             * @param params 打开文件时的参数列表。
             */
            explicit VideoCapture(const String &filename, int apiPreference, const KKMediaFormat &params);

            /**
             * @brief 用指定的文件名、API偏好和参数打开视频源。
             *
             * @param filename 要打开的视频文件名。
             * @param apiPreference 优选的视频捕获API。
             * @param params 打开文件时的参数列表。
             * @return 如果成功打开返回 true，否则返回 false。
             */
            virtual bool open(const String &filename, int apiPreference, const KKMediaFormat &params);

            /**
             * @brief 用指定的设备索引、API偏好和参数打开视频源。
             *
             * @param index 要捕获的设备索引。
             * @param apiPreference 优选的视频捕获API。
             * @param params 打开设备时的参数列表。
             * @return 如果成功打开返回 true，否则返回 false。
             */
            virtual bool open(int index, int apiPreference, const KKMediaFormat &params);

            /**
             * @brief 检查视频源是否已经打开。
             *
             * @return 如果视频源已经打开返回 true，否则返回 false。
             */
            virtual bool isOpened() const;

            /**
             * @brief 释放与 VideoCapture 关联的所有资源。
             */
            virtual void release();

            /**
             * @brief VideoCapture 的虚析构函数，确保适当地清除资源。
             */
            virtual ~VideoCapture();

            /**
             * @brief  视频采集到的回调函数
             */
            virtual void IncomingFrame(uint8_t *videoFrame,
                                       size_t videoFrameLength,
                                       const KKVideoCapConfig &frame, int64_t captureTime = 0);

            // Call backs
            virtual void RegisterCaptureDataCallback(
                    kkrtc::VideoSinkInterface<VideoFrame> *dataCallback) override;

            virtual void DeRegisterCaptureDataCallback() override;

        private:
            int32_t DeliverCapturedFrame(VideoFrame& captureFrame);
            std::mutex cap_lock_;
            kkrtc::VideoSinkInterface<VideoFrame> *dataCallback_;
            KKPtr<kkrtc::PluginGlobParam_t> plugin_glob_params_;

        protected:
            /**
             * @brief 保护的成员变量，存储视频捕获的内部实现接口。
             */
            KKPtr<IVideoCapture> cap_;
        };
    }//namespace vcap
}//namespace kkrtc
#endif //KKRTC_KKRTC_CAP_API_H
