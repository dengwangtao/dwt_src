#include "Logger.h"

#include <iostream>
#include <chrono>

namespace dwt {

// 获取单例对象
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

// 设置日志级别
void Logger::setLogLevel(LogLevel level) {
    m_logLevel = level;
}

const char* LogLevelName[(int)LogLevel::NUM_LOG_LEVELS] =
{
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

// 写日志
void Logger::log(std::string msg) {

    /**
     * [级别信息] 20240528 09:46:48 : msg
    */

    // 日志等级
    std::cout << "[" << LogLevelName[(int)m_logLevel] << "] ";

    std::cout << now_str() << " : " << msg << std::endl;

}


std::string Logger::now_str() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();
    // 将 time_point 转换为 time_t 类型
    std::time_t time_t_value = std::chrono::system_clock::to_time_t(now);
    
    // 将 time_t 转换为 tm 结构
    std::tm tm_time = *std::localtime(&time_t_value);
    
    char buf[128] = {0};
    snprintf(buf, sizeof(buf), "%4d/%02d/%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    return buf;
}


}