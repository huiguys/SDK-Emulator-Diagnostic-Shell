#pragma once

#include <cstdint>
#include <string>

namespace ti_sdk {

enum class PinMode { INPUT, OUTPUT, INPUT_PULLUP, INPUT_PULLDOWN };

enum class PinState { LOW, HIGH };

class GPIO {
public:
  // Initialize GPIO subsystem
  static bool initialize();

  // Configure a pin
  static bool configurePin(uint8_t port, uint8_t pin, PinMode mode);

  // Write to a pin
  static bool writePin(uint8_t port, uint8_t pin, PinState state);

  // Read from a pin
  static PinState readPin(uint8_t port, uint8_t pin);

  // Toggle pin state
  static bool togglePin(uint8_t port, uint8_t pin);

  // Save current GPIO state to JSON
  static std::string saveState();

  // Restore GPIO state from JSON
  static bool restoreState(const std::string &state);

private:
  GPIO() = delete; // Prevent instantiation
};

} // namespace ti_sdk