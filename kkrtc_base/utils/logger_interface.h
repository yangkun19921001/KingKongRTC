//
// Created by devyk on 2023/9/17.
//

#ifndef KKRTC_LOGGER_INTERFACE_H
#define KKRTC_LOGGER_INTERFACE_H

#include <string>
#include <memory>
#include <iostream>
#include <fstream>

namespace kkrtc {
    namespace utils {
        namespace log {

            enum class LoggerId{
                NONE=0,
                SPDLOG = 1,
            };

            // 定义不同的日志级别
            enum class LogLevel {
                TRACE,      // 追踪信息
                DEBUG,      // 调试信息
                INFO,       // 一般信息
                WARNING,    // 警告信息
                ERR,      // 错误信息
            };

            class ILogger {
            public:
                LogLevel logLevel_;
                bool consoleOutputEnabled_;
                bool fileOutputEnabled_;
                std::string filepath_;
            public:
                virtual ~ILogger() = default;

                virtual void log(LogLevel level,const std::string& tag, const std::string& message) = 0;

                // 设置日志级别
                virtual void setLogLevel(LogLevel level) {
                        this->logLevel_  = level;
                };

                // 设置是否输出到控制台
                virtual void setConsoleOutput(bool enable) {
                    this->consoleOutputEnabled_  = enable;
                };

                // 设置是否输出到日志文件
                virtual void setFileOutput(bool enable, const std::string& filepath) {
                    this->fileOutputEnabled_  = enable;
                    this->filepath_ = filepath;
                };

            };
        }
    } //utils
}//namespace kkrtc




#endif //KKRTC_LOGGER_INTERFACE_H
