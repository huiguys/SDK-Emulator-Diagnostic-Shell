#include "adc.hpp"
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>
#include <unordered_map>


using json = nlohmann::json;

namespace ti_sdk {

namespace {
struct ChannelConfig {
  bool configured;
  uint32_t sampleRate;
  uint16_t lastValue;
  void (*callback)(uint16_t);
  std::thread samplingThread;
  bool continuousSampling;
};

std::unordered_map<uint8_t, ChannelConfig> channels;
std::mutex adc_mutex;
bool initialized = false;

void samplingLoop(uint8_t channel) {
  auto &config = channels[channel];
  while (config.continuousSampling) {
    uint16_t value = ADC::read(channel);
    if (config.callback) {
      config.callback(value);
    }
    std::this_thread::sleep_for(
        std::chrono::microseconds(1000000 / config.sampleRate));
  }
}
} // namespace

bool ADC::initialize() {
  std::lock_guard<std::mutex> lock(adc_mutex);
  initialized = true;
  channels.clear();
  return true;
}

bool ADC::configureChannel(uint8_t channel, uint32_t sampleRate) {
  if (!initialized)
    return false;

  std::lock_guard<std::mutex> lock(adc_mutex);
  channels[channel] = ChannelConfig{
      true,          // configured
      sampleRate,    // sampleRate
      0,             // lastValue
      nullptr,       // callback
      std::thread(), // samplingThread
      false          // continuousSampling
  };
  return true;
}

uint16_t ADC::read(uint8_t channel) {
  if (!initialized)
    return 0;

  std::lock_guard<std::mutex> lock(adc_mutex);
  auto it = channels.find(channel);
  if (it == channels.end() || !it->second.configured)
    return 0;

  // Simulate ADC reading with some noise
  static uint16_t baseValue = 2048; // 12-bit ADC mid-point
  it->second.lastValue = baseValue + (rand() % 100) - 50;
  return it->second.lastValue;
}

uint16_t ADC::readAverage(uint8_t channel, uint8_t samples) {
  if (!initialized || samples == 0)
    return 0;

  uint32_t sum = 0;
  for (uint8_t i = 0; i < samples; ++i) {
    sum += read(channel);
  }
  return sum / samples;
}

bool ADC::startContinuous(uint8_t channel, void (*callback)(uint16_t)) {
  if (!initialized)
    return false;

  std::lock_guard<std::mutex> lock(adc_mutex);
  auto it = channels.find(channel);
  if (it == channels.end() || !it->second.configured)
    return false;

  if (it->second.continuousSampling) {
    stopContinuous(channel);
  }

  it->second.callback = callback;
  it->second.continuousSampling = true;
  it->second.samplingThread = std::thread(samplingLoop, channel);
  return true;
}

bool ADC::stopContinuous(uint8_t channel) {
  if (!initialized)
    return false;

  std::lock_guard<std::mutex> lock(adc_mutex);
  auto it = channels.find(channel);
  if (it == channels.end() || !it->second.configured)
    return false;

  if (it->second.continuousSampling) {
    it->second.continuousSampling = false;
    if (it->second.samplingThread.joinable()) {
      it->second.samplingThread.join();
    }
  }
  return true;
}

std::string ADC::saveState() {
  std::lock_guard<std::mutex> lock(adc_mutex);

  json state;
  state["initialized"] = initialized;

  json channelsState;
  for (const auto &[channel, config] : channels) {
    if (config.configured) {
      channelsState[std::to_string(channel)] = {
          {"sampleRate", config.sampleRate}, {"lastValue", config.lastValue}};
    }
  }
  state["channels"] = channelsState;

  return state.dump();
}

bool ADC::restoreState(const std::string &state_str) {
  try {
    auto state = json::parse(state_str);

    std::lock_guard<std::mutex> lock(adc_mutex);
    initialized = state["initialized"];

    channels.clear();
    auto channelsState = state["channels"];
    for (auto it = channelsState.begin(); it != channelsState.end(); ++it) {
      uint8_t channel = std::stoi(it.key());
      channels[channel] = ChannelConfig{
          true,                     // configured
          it.value()["sampleRate"], // sampleRate
          it.value()["lastValue"],  // lastValue
          nullptr,                  // callback
          std::thread(),            // samplingThread
          false                     // continuousSampling
      };
    }

    return true;
  } catch (const std::exception &) {
    return false;
  }
}

} // namespace ti_sdk