//
// Created by devyk on 2023/9/17.
//

#include "spd_logger.h"

namespace kkrtc {
    namespace utils {
        namespace log {

            SpdlogLogger::~SpdlogLogger() {
            }

            SpdlogLogger::SpdlogLogger() {
                spdlog::init_thread_pool(8192, 1);
                std::vector<spdlog::sink_ptr> sinks;
                logger_ = std::make_shared<spdlog::async_logger>("kkrtc_logger", begin(sinks), end(sinks),
                                                                 spdlog::thread_pool(),
                                                                 spdlog::async_overflow_policy::block);
            }

            SpdlogLogger::SpdlogLogger(std::string module_name) {
                this->modulename_ = module_name;
                spdlog::init_thread_pool(8192, 1);
                std::vector<spdlog::sink_ptr> sinks;
                logger_ = std::make_shared<spdlog::async_logger>(module_name, begin(sinks), end(sinks),
                                                                 spdlog::thread_pool(),
                                                                 spdlog::async_overflow_policy::block);
            }


            void SpdlogLogger::setFileOutput(bool enable, const std::string &filepath) {
                this->fileOutputEnabled_ = enable;
                this->filepath_ = filepath;
                if (enable) {
                    auto now = std::chrono::system_clock::now();
                    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
                    std::tm local_time;
                    localtime_s(&local_time, &now_time);
                    char date_buffer[64];

                    std::string str;
                    str.append("kkrtc-sdk-")
                            .append(modulename_.empty() ? "" : modulename_+"_")
                            .append("%Y-%m-%d.log");
                    auto log_name = str.c_str();
                    std::strftime(date_buffer, sizeof(date_buffer), log_name, &local_time);
                    std::string file_name(date_buffer);

                    auto max_file_size = 10 * 1024 * 1024;  // 10MB
                    auto max_files = 10;  // You can adjust this value based on your needs
                    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filepath + "/" + file_name,
                                                                                            max_file_size, max_files);

                    logger_->sinks().push_back(file_sink);
                }
            }

            void SpdlogLogger::log(LogLevel level, const std::string &tag, const std::string &message) {
                switch (level) {
                    case LogLevel::TRACE:
                        logger_->trace("[{}] {}", tag, message);
                        break;
                    case LogLevel::DEBUG:
                        logger_->debug("[{}] {}", tag, message);
                        break;
                    case LogLevel::INFO:
                        logger_->info("[{}] {}", tag, message);
                        break;
                    case LogLevel::WARNING:
                        logger_->warn("[{}] {}", tag, message);
                        break;
                    case LogLevel::ERR:
                        logger_->error("[{}] {}", tag, message);
                        break;

                }
            }

            void SpdlogLogger::setLogLevel(LogLevel level) {
                ILogger::setLogLevel(level);
                switch (level) {
                    case LogLevel::TRACE:
                        logger_->set_level(spdlog::level::trace);
                        break;
                    case LogLevel::DEBUG:
                        logger_->set_level(spdlog::level::debug);
                        break;
                    case LogLevel::INFO:
                        logger_->set_level(spdlog::level::info);
                        break;
                    case LogLevel::WARNING:
                        logger_->set_level(spdlog::level::warn);
                        break;
                    case LogLevel::ERR:
                        logger_->set_level(spdlog::level::err);
                        break;
                }
            }

            void SpdlogLogger::setConsoleOutput(bool enable) {
                ILogger::setConsoleOutput(enable);
                if (enable) {
                    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                    logger_->sinks().push_back(console_sink);
                }
            }
        }
    }
}