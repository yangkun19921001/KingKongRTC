//
// Created by devyk on 2023/9/17.
//

#ifndef KKRTC_DEF_LOGGER_H
#define KKRTC_DEF_LOGGER_H
#include "logger_interface.h"

namespace kkrtc {
    namespace utils {
        namespace log {
            class DefLogger :public ILogger{
            public:
                DefLogger();
                ~DefLogger();

                virtual void log(LogLevel level,const std::string& tag, const std::string& message) override;

                virtual void setFileOutput(bool enable ,const std::string& filepath) override;
            private:
                std::ofstream logFile;

            };

        }
    }
}


#endif //KKRTC_DEF_LOGGER_H
