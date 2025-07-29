#include "uart.hpp"
#include <mutex>
#include <nlohmann/json.hpp>
#include <queue>
#include <thread>


using json = nlohmann::json;

namespace ti_sdk {

namespace {
struct UARTState {
  bool initialized;
  uint32_t baudRate;
  std::queue<uint8_t> rxBuffer;
  std::queue<uint8_t> txBuffer;
  std::mutex mutex;
};

UARTState uart_state;
} // namespace

bool UART::initialize(uint32_t baudRate) {
  std::lock_guard<std::mutex> lock(uart_state.mutex);
  uart_state.initialized = true;
  uart_state.baudRate = baudRate;
  return true;
}

bool UART::write(uint8_t data) {
  if (!uart_state.initialized)
    return false;

  std::lock_guard<std::mutex> lock(uart_state.mutex);
  uart_state.txBuffer.push(data);
  return true;
}

bool UART::read(uint8_t &data) {
  if (!uart_state.initialized)
    return false;

  std::lock_guard<std::mutex> lock(uart_state.mutex);
  if (uart_state.rxBuffer.empty())
    return false;

  data = uart_state.rxBuffer.front();
  uart_state.rxBuffer.pop();
  return true;
}

bool UART::available() {
  std::lock_guard<std::mutex> lock(uart_state.mutex);
  return !uart_state.rxBuffer.empty();
}

std::string UART::saveState() {
  std::lock_guard<std::mutex> lock(uart_state.mutex);

  json state;
  state["initialized"] = uart_state.initialized;
  state["baudRate"] = uart_state.baudRate;

  // Save RX buffer
  std::vector<uint8_t> rxData;
  auto rxTemp = uart_state.rxBuffer;
  while (!rxTemp.empty()) {
    rxData.push_back(rxTemp.front());
    rxTemp.pop();
  }
  state["rxBuffer"] = rxData;

  // Save TX buffer
  std::vector<uint8_t> txData;
  auto txTemp = uart_state.txBuffer;
  while (!txTemp.empty()) {
    txData.push_back(txTemp.front());
    txTemp.pop();
  }
  state["txBuffer"] = txData;

  return state.dump();
}

bool UART::restoreState(const std::string &state_str) {
  try {
    auto state = json::parse(state_str);

    std::lock_guard<std::mutex> lock(uart_state.mutex);

    uart_state.initialized = state["initialized"];
    uart_state.baudRate = state["baudRate"];

    // Clear and restore RX buffer
    while (!uart_state.rxBuffer.empty())
      uart_state.rxBuffer.pop();
    for (const auto &byte : state["rxBuffer"]) {
      uart_state.rxBuffer.push(byte);
    }

    // Clear and restore TX buffer
    while (!uart_state.txBuffer.empty())
      uart_state.txBuffer.pop();
    for (const auto &byte : state["txBuffer"]) {
      uart_state.txBuffer.push(byte);
    }

    return true;
  } catch (const std::exception &) {
    return false;
  }
}

} // namespace ti_sdk