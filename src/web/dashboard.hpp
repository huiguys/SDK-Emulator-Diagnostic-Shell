#pragma once

#include "../sdk/logger.hpp"
#include <functional>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>


namespace ti_sdk {
namespace web {

class Dashboard {
public:
  static Dashboard &getInstance() {
    static Dashboard instance;
    return instance;
  }

  void start(uint16_t port = 8080) {
    if (running_)
      return;

    port_ = port;
    running_ = true;
    serverThread_ = std::thread(&Dashboard::runServer, this);
    LOG_INFO("Web dashboard started on port " + std::to_string(port));
  }

  void stop() {
    if (!running_)
      return;

    running_ = false;
    if (serverThread_.joinable()) {
      serverThread_.join();
    }
    LOG_INFO("Web dashboard stopped");
  }

  // Update GPIO state
  void updateGPIO(uint8_t port, uint8_t pin, bool state) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    gpioStates_[std::to_string(port) + ":" + std::to_string(pin)] = state;
    notifyClients("gpio");
  }

  // Update UART data
  void updateUART(uint8_t channel, const std::string &data) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    uartData_[channel].append(data);
    if (uartData_[channel].length() > maxUartBufferSize_) {
      uartData_[channel] = uartData_[channel].substr(
          uartData_[channel].length() - maxUartBufferSize_);
    }
    notifyClients("uart");
  }

  // Update ADC value
  void updateADC(uint8_t channel, uint16_t value) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    adcValues_[channel] = value;
    notifyClients("adc");
  }

  // Get current state as JSON
  std::string getStateJSON() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    nlohmann::json state;
    state["gpio"] = gpioStates_;
    state["uart"] = uartData_;
    state["adc"] = adcValues_;
    return state.dump();
  }

private:
  Dashboard() : running_(false), port_(8080), maxUartBufferSize_(1024) {}
  ~Dashboard() { stop(); }

  void runServer();
  void handleWebSocket(const std::string &message);
  void notifyClients(const std::string &type);

  std::thread serverThread_;
  bool running_;
  uint16_t port_;

  mutable std::mutex dataMutex_;
  std::map<std::string, bool> gpioStates_;
  std::map<uint8_t, std::string> uartData_;
  std::map<uint8_t, uint16_t> adcValues_;
  const size_t maxUartBufferSize_;
};

} // namespace web
} // namespace ti_sdk