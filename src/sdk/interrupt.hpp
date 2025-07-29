#pragma once

#include "logger.hpp"
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <thread>


namespace ti_sdk {

enum class InterruptType {
  GPIO_RISING,
  GPIO_FALLING,
  GPIO_CHANGE,
  TIMER,
  ADC_COMPLETE,
  UART_RX,
  UART_TX
};

class InterruptManager {
public:
  using InterruptHandler = std::function<void()>;

  static InterruptManager &getInstance() {
    static InterruptManager instance;
    return instance;
  }

  // Register an interrupt handler
  bool attachInterrupt(InterruptType type, uint8_t source,
                       InterruptHandler handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t id = makeInterruptId(type, source);
    handlers_[id] = handler;
    LOG_INFO("Interrupt handler attached for type: " +
             std::to_string(static_cast<int>(type)) +
             ", source: " + std::to_string(source));
    return true;
  }

  // Remove an interrupt handler
  bool detachInterrupt(InterruptType type, uint8_t source) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t id = makeInterruptId(type, source);
    auto it = handlers_.find(id);
    if (it != handlers_.end()) {
      handlers_.erase(it);
      LOG_INFO("Interrupt handler detached for type: " +
               std::to_string(static_cast<int>(type)) +
               ", source: " + std::to_string(source));
      return true;
    }
    return false;
  }

  // Trigger an interrupt
  void triggerInterrupt(InterruptType type, uint8_t source) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t id = makeInterruptId(type, source);
    auto it = handlers_.find(id);
    if (it != handlers_.end()) {
      pendingInterrupts_.push(it->second);
      LOG_DEBUG("Interrupt triggered for type: " +
                std::to_string(static_cast<int>(type)) +
                ", source: " + std::to_string(source));
    }
  }

  // Process pending interrupts
  void processInterrupts() {
    while (running_) {
      InterruptHandler handler;
      {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!pendingInterrupts_.empty()) {
          handler = pendingInterrupts_.front();
          pendingInterrupts_.pop();
        }
      }
      if (handler) {
        handler();
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  void start() {
    running_ = true;
    processingThread_ = std::thread(&InterruptManager::processInterrupts, this);
  }

  void stop() {
    running_ = false;
    if (processingThread_.joinable()) {
      processingThread_.join();
    }
  }

private:
  InterruptManager() : running_(false) {}
  ~InterruptManager() { stop(); }

  uint32_t makeInterruptId(InterruptType type, uint8_t source) {
    return (static_cast<uint32_t>(type) << 8) | source;
  }

  std::map<uint32_t, InterruptHandler> handlers_;
  std::queue<InterruptHandler> pendingInterrupts_;
  std::mutex mutex_;
  std::thread processingThread_;
  bool running_;
};

} // namespace ti_sdk