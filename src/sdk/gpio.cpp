#include "gpio.hpp"
#include <mutex>
#include <nlohmann/json.hpp>
#include <unordered_map>


using json = nlohmann::json;

namespace ti_sdk {

namespace {
struct PinConfig {
  PinMode mode;
  PinState state;
};

using PinId = uint32_t;

PinId makePinId(uint8_t port, uint8_t pin) {
  return (static_cast<uint32_t>(port) << 8) | pin;
}

std::unordered_map<PinId, PinConfig> pin_configs;
std::mutex gpio_mutex;
bool initialized = false;
} // namespace

bool GPIO::initialize() {
  std::lock_guard<std::mutex> lock(gpio_mutex);
  initialized = true;
  pin_configs.clear();
  return true;
}

bool GPIO::configurePin(uint8_t port, uint8_t pin, PinMode mode) {
  if (!initialized)
    return false;

  std::lock_guard<std::mutex> lock(gpio_mutex);
  pin_configs[makePinId(port, pin)] = PinConfig{mode, PinState::LOW};
  return true;
}

bool GPIO::writePin(uint8_t port, uint8_t pin, PinState state) {
  if (!initialized)
    return false;

  std::lock_guard<std::mutex> lock(gpio_mutex);
  auto id = makePinId(port, pin);
  auto it = pin_configs.find(id);

  if (it == pin_configs.end() || it->second.mode != PinMode::OUTPUT) {
    return false;
  }

  it->second.state = state;
  return true;
}

PinState GPIO::readPin(uint8_t port, uint8_t pin) {
  if (!initialized)
    return PinState::LOW;

  std::lock_guard<std::mutex> lock(gpio_mutex);
  auto id = makePinId(port, pin);
  auto it = pin_configs.find(id);

  if (it == pin_configs.end()) {
    return PinState::LOW;
  }

  return it->second.state;
}

bool GPIO::togglePin(uint8_t port, uint8_t pin) {
  if (!initialized)
    return false;

  std::lock_guard<std::mutex> lock(gpio_mutex);
  auto id = makePinId(port, pin);
  auto it = pin_configs.find(id);

  if (it == pin_configs.end() || it->second.mode != PinMode::OUTPUT) {
    return false;
  }

  it->second.state =
      (it->second.state == PinState::HIGH) ? PinState::LOW : PinState::HIGH;
  return true;
}

std::string GPIO::saveState() {
  std::lock_guard<std::mutex> lock(gpio_mutex);

  json state;
  state["initialized"] = initialized;

  json pins;
  for (const auto &[id, config] : pin_configs) {
    uint8_t port = id >> 8;
    uint8_t pin = id & 0xFF;

    pins[std::to_string(id)] = {{"port", port},
                                {"pin", pin},
                                {"mode", static_cast<int>(config.mode)},
                                {"state", static_cast<int>(config.state)}};
  }
  state["pins"] = pins;

  return state.dump();
}

bool GPIO::restoreState(const std::string &state_str) {
  try {
    auto state = json::parse(state_str);

    std::lock_guard<std::mutex> lock(gpio_mutex);
    initialized = state["initialized"];

    pin_configs.clear();
    auto pins = state["pins"];
    for (auto it = pins.begin(); it != pins.end(); ++it) {
      uint8_t port = it.value()["port"];
      uint8_t pin = it.value()["pin"];
      PinMode mode = static_cast<PinMode>(it.value()["mode"]);
      PinState state = static_cast<PinState>(it.value()["state"]);

      pin_configs[makePinId(port, pin)] = PinConfig{mode, state};
    }

    return true;
  } catch (const std::exception &) {
    return false;
  }
}

} // namespace ti_sdk