#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ti_sdk {

class ADC {
public:
  // Initialize ADC subsystem
  static bool initialize();

  // Configure ADC channel
  static bool configureChannel(uint8_t channel, uint32_t sampleRate);

  // Read ADC value
  static uint16_t read(uint8_t channel);

  // Read ADC value with averaging
  static uint16_t readAverage(uint8_t channel, uint8_t samples);

  // Start continuous sampling
  static bool startContinuous(uint8_t channel, void (*callback)(uint16_t));

  // Stop continuous sampling
  static bool stopContinuous(uint8_t channel);

  // Save current ADC state to JSON
  static std::string saveState();

  // Restore ADC state from JSON
  static bool restoreState(const std::string &state);

private:
  ADC() = delete; // Prevent instantiation
};

} // namespace ti_sdk