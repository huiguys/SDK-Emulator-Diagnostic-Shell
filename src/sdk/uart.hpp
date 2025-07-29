#pragma once

#include <cstdint>
#include <string>

namespace ti_sdk {

class UART {
public:
  // Initialize UART with specified baud rate
  static bool initialize(uint32_t baudRate);

  // Write a byte to UART
  static bool write(uint8_t data);

  // Read a byte from UART
  static bool read(uint8_t &data);

  // Check if data is available to read
  static bool available();

  // Save current UART state to JSON
  static std::string saveState();

  // Restore UART state from JSON
  static bool restoreState(const std::string &state);

private:
  UART() = delete; // Prevent instantiation
};

} // namespace ti_sdk