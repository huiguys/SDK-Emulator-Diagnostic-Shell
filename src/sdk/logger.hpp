#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

namespace ti_sdk {

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

class Logger {
public:
  static Logger &getInstance() {
    static Logger instance;
    return instance;
  }

  void setLogFile(const std::string &filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open()) {
      logFile_.close();
    }
    logFile_.open(filename, std::ios::app);
  }

  void log(LogLevel level, const std::string &message) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string timestamp = getCurrentTimestamp();
    std::string levelStr = getLevelString(level);

    std::stringstream ss;
    ss << timestamp << " [" << levelStr << "] " << message << std::endl;

    if (logFile_.is_open()) {
      logFile_ << ss.str();
      logFile_.flush();
    }
    std::cout << ss.str();
  }

private:
  Logger() = default;
  ~Logger() {
    if (logFile_.is_open()) {
      logFile_.close();
    }
  }

  std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
  }

  std::string getLevelString(LogLevel level) {
    switch (level) {
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARNING:
      return "WARNING";
    case LogLevel::ERROR:
      return "ERROR";
    default:
      return "UNKNOWN";
    }
  }

  std::mutex mutex_;
  std::ofstream logFile_;
};

#define LOG_DEBUG(msg) Logger::getInstance().log(LogLevel::DEBUG, msg)
#define LOG_INFO(msg) Logger::getInstance().log(LogLevel::INFO, msg)
#define LOG_WARNING(msg) Logger::getInstance().log(LogLevel::WARNING, msg)
#define LOG_ERROR(msg) Logger::getInstance().log(LogLevel::ERROR, msg)

} // namespace ti_sdk