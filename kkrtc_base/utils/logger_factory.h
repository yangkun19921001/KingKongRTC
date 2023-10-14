//
// Created by devyk on 2023/9/17.
//

#ifndef KKRTC_LOGGER_FACTORY_H
#define KKRTC_LOGGER_FACTORY_H

#include <sstream>
#include "logger_interface.h"
#include "spd_logger.h"
#include "def_logger.h"
#include "kkrtc_std_wrapper.h"

namespace kkrtc {
    namespace utils {
        namespace log {

            class LogStream {
            public:
                LogStream(LogLevel level, std::shared_ptr<ILogger> logger, const char *file, int line)
                        : level_(level), logger_(logger), file_(file), line_(line), tag_("") {

                }

                LogStream(LogLevel level, std::shared_ptr<ILogger> logger, const char *tag, const char *file, int line)
                        : level_(level), logger_(logger), file_(file), line_(line), tag_(tag) {}

                template<typename T>
                LogStream &operator<<(const T &msg) {
                    stream_ << msg;
                    return *this;
                }

                ~LogStream() {
                    std::string finalMsg =
                            "[" + std::string(file_) + ":" + std::to_string(line_) + "] " + stream_.str();
                    if (logger_)
                        logger_->log(level_, tag_, finalMsg);
                }

            private:
                LogLevel level_;
                std::ostringstream stream_;
                std::shared_ptr<ILogger> logger_;
                const char *file_;
                const char *tag_;
                int line_;

            };


            class LoggerFactory {
            public:
                static KKPtr<ILogger> createLogger(LoggerId id) {
                    switch (id) {
                        case LoggerId::NONE:
                            return makePtr<DefLogger>();
                        case LoggerId::SPDLOG:
                            return makePtr<SpdlogLogger>();
                    }
                    return makePtr<SpdlogLogger>();
                };
            };

        }
    }
}
#endif //KKRTC_LOGGER_FACTORY_H
