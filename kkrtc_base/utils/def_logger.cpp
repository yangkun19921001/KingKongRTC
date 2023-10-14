//
// Created by devyk on 2023/9/17.
//

#include "def_logger.h"
#include <chrono>
#include <ctime>

namespace kkrtc {
    namespace utils {
        namespace log {

            DefLogger::DefLogger() {

            }

            DefLogger::~DefLogger() {
                if (logFile.is_open()) {
                    logFile.close();
                }
            }

            void DefLogger::log(LogLevel level,const std::string& tag, const std::string& message) {
                if (level >= logLevel_) {
                    auto now = std::chrono::system_clock::now();
                    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
                    std::tm local_time;
                    localtime_s(&local_time, &now_time);
                    char date_buffer[64];
                    std::strftime(date_buffer, sizeof(date_buffer), "[%Y-%m-%d %H:%M:%S] ", &local_time);
                    std::string log_message = std::string(date_buffer) + message;

                    if (consoleOutputEnabled_) {
                        std::cout << log_message << std::endl;
                    }
                    if (logFile.is_open()) {
                        logFile << log_message << std::endl;
                    }
                }
            }

            void DefLogger::setFileOutput(bool enable, const std::string &filepath) {
                if (logFile.is_open()) {
                    logFile.close();
                }
                auto now = std::chrono::system_clock::now();
                std::time_t now_time = std::chrono::system_clock::to_time_t(now);
                std::tm local_time;
                localtime_s(&local_time, &now_time);
                char date_buffer[64];
                std::strftime(date_buffer, sizeof(date_buffer), "kkrtc-sdk-%Y-%m-%d.log", &local_time);
                std::string file_name(date_buffer);
                //std::ofstream outputFile(filepath+file_name.c_str(), std::ios::app);
                logFile.open(filepath+"/"+file_name.c_str(),std::ios::app);
            }

        } // log
    } // utils
} // kkrtc
