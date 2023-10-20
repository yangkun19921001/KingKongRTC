//
// Created by devyk on 2023/9/17.
//

#ifndef KKRTC_LOG_HELPER_H
#define KKRTC_LOG_HELPER_H

#include "logger_factory.h"
#include "kkrtc_std_wrapper.h"

void InitGlobLogger(kkrtc::KKPtr<kkrtc::utils::log::ILogger> logger);
void InitPluginLogger(kkrtc::KKPtr<kkrtc::utils::log::ILogger> logger);

kkrtc::KKPtr<kkrtc::utils::log::ILogger> GetGlobLogger();

#define globalLogger GetGlobLogger()
#define KKLogConfig_InitGlobLogger(logger) InitGlobLogger(logger);
#define KKLogConfig_InitPluginLogger(logger) InitPluginLogger(logger);
#define KKLogConfig_ReleaseGlobLogger() if(!globalLogger.empty()) globalLogger.release();
#define KKLogConfig_SetLogLevel(level) if(globalLogger.empty()) std::cout << "Error: globalLogger not init." <<std::endl; else globalLogger->setLogLevel(level);
#define KKLogConfig_SetConsoleOutput(enable) if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else  globalLogger->setConsoleOutput(enable);
#define KKLogConfig_SetFileOutput(enable, filepath)  if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else globalLogger->setFileOutput(enable,filepath);

#define KKLogInfo  if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else  kkrtc::utils::log::LogStream(kkrtc::utils::log::LogLevel::INFO, globalLogger,__FILE__, __LINE__)
#define KKLogDebug  if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else kkrtc::utils::log::LogStream(kkrtc::utils::log::LogLevel::DEBUG, globalLogger,__FILE__, __LINE__)
#define KKLogError  if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else kkrtc::utils::log::LogStream(kkrtc::utils::log::LogLevel::ERR, globalLogger,__FILE__, __LINE__)
#define KKLogTrace  if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else kkrtc::utils::log::LogStream(kkrtc::utils::log::LogLevel::TRACE, globalLogger,__FILE__, __LINE__)
#define KKLogWarn  if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else kkrtc::utils::log::LogStream(kkrtc::utils::log::LogLevel::WARNING, globalLogger,__FILE__, __LINE__)

#define KKLogInfoTag(tag)   if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else kkrtc::utils::log::LogStream(kkrtc::utils::log::LogLevel::INFO, globalLogger, tag, __FILE__, __LINE__)
#define KKLogDebugTag(tag)  if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else kkrtc::utils::log::LogStream(kkrtc::utils::log::LogLevel::DEBUG, globalLogger, tag, __FILE__, __LINE__)
#define KKLogErrorTag(tag)  if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else kkrtc::utils::log::LogStream(kkrtc::utils::log::LogLevel::ERR, globalLogger, tag, __FILE__, __LINE__)
#define KKLogTraceTag(tag)  if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else kkrtc::utils::log::LogStream(kkrtc::utils::log::LogLevel::TRACE, globalLogger, tag, __FILE__, __LINE__)
#define KKLogWarnTag(tag)   if(!globalLogger) std::cout << "Error: globalLogger not init." <<std::endl; else kkrtc::utils::log::LogStream(kkrtc::utils::log::LogLevel::WARNING, globalLogger,tag, __FILE__, __LINE__)

#define PLUGIN_LOG(level, module_name, msg, log_observer) if(log_observer) log_observer->OnLogMessage(kkrtc::utils::log::LogLevel::level, module_name, msg);

#define PLUGIN_LOG_ERR(module_name,  msg, log_observer) \
    PLUGIN_LOG(ERR, module_name, msg, log_observer)

#define PLUGIN_LOG_WARN(module_name,  msg, log_observer) \
    PLUGIN_LOG(WARNING, module_name,  msg, log_observer)

#define PLUGIN_LOG_DEBUG(module_name,  msg, log_observer) \
    PLUGIN_LOG(DEBUG, module_name,  msg, log_observer)

#define PLUGIN_LOG_INFO(module_name,  msg, log_observer) \
    PLUGIN_LOG(INFO, module_name,  msg, log_observer)

#endif //KKRTC_LOG_HELPER_H
