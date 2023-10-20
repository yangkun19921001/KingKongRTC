//
// Created by devyk on 2023/9/17.
//

#ifndef KKRTC_SPD_LOGGER_H
#define KKRTC_SPD_LOGGER_H

#include "logger_interface.h"

#include <spdlog/include/spdlog/spdlog.h>
#include <spdlog/include/spdlog/sinks/basic_file_sink.h>
#include <spdlog/include/spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/include/spdlog/async.h>
#include <spdlog/include/spdlog/sinks/rotating_file_sink.h>

namespace kkrtc {
    namespace utils {
        namespace log {
            class SpdlogLogger : public ILogger {
            public:
                virtual ~SpdlogLogger();

                SpdlogLogger();
                SpdlogLogger(std::string module_name);

                virtual void log(LogLevel level,const std::string& tag, const std::string& message) override;

                // 设置日志级别
                virtual void setLogLevel(LogLevel level)override;

                // 设置是否输出到控制台
                virtual void setConsoleOutput(bool enable)override;

                // 设置是否输出到日志文件
                 void setFileOutput(bool enable, const std::string& filename) override;
            private:
                std::shared_ptr<spdlog::logger> logger_;
            };
        }
    }
}


#endif //KKRTC_SPD_LOGGER_H
