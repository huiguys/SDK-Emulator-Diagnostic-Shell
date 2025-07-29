#pragma once

#include "logger.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>


namespace ti_sdk {

struct GPIOConfig {
  uint8_t numPorts;
  uint8_t pinsPerPort;
  bool hasInterrupts;
  bool hasPullUp;
  bool hasPullDown;
};

struct UARTConfig {
  uint8_t numChannels;
  std::vector<uint32_t> supportedBaudRates;
  bool hasFlowControl;
  size_t txBufferSize;
  size_t rxBufferSize;
};

struct ADCConfig {
  uint8_t numChannels;
  uint8_t resolution;
  uint32_t maxSampleRate;
  bool hasAutoTrigger;
  bool hasDMA;
};

class DeviceProfile {
public:
  DeviceProfile(const std::string &name) : name_(name) {}

  void setGPIOConfig(const GPIOConfig &config) {
    gpioConfig_ = config;
    LOG_INFO("GPIO configuration set for device: " + name_);
  }

  void setUARTConfig(const UARTConfig &config) {
    uartConfig_ = config;
    LOG_INFO("UART configuration set for device: " + name_);
  }

  void setADCConfig(const ADCConfig &config) {
    adcConfig_ = config;
    LOG_INFO("ADC configuration set for device: " + name_);
  }

  const GPIOConfig &getGPIOConfig() const { return gpioConfig_; }
  const UARTConfig &getUARTConfig() const { return uartConfig_; }
  const ADCConfig &getADCConfig() const { return adcConfig_; }
  const std::string &getName() const { return name_; }

  std::string toJSON() const {
    nlohmann::json j;
    j["name"] = name_;

    j["gpio"] = {{"numPorts", gpioConfig_.numPorts},
                 {"pinsPerPort", gpioConfig_.pinsPerPort},
                 {"hasInterrupts", gpioConfig_.hasInterrupts},
                 {"hasPullUp", gpioConfig_.hasPullUp},
                 {"hasPullDown", gpioConfig_.hasPullDown}};

    j["uart"] = {{"numChannels", uartConfig_.numChannels},
                 {"supportedBaudRates", uartConfig_.supportedBaudRates},
                 {"hasFlowControl", uartConfig_.hasFlowControl},
                 {"txBufferSize", uartConfig_.txBufferSize},
                 {"rxBufferSize", uartConfig_.rxBufferSize}};

    j["adc"] = {{"numChannels", adcConfig_.numChannels},
                {"resolution", adcConfig_.resolution},
                {"maxSampleRate", adcConfig_.maxSampleRate},
                {"hasAutoTrigger", adcConfig_.hasAutoTrigger},
                {"hasDMA", adcConfig_.hasDMA}};

    return j.dump(4);
  }

  static DeviceProfile fromJSON(const std::string &json) {
    auto j = nlohmann::json::parse(json);
    DeviceProfile profile(j["name"]);

    GPIOConfig gpio{j["gpio"]["numPorts"], j["gpio"]["pinsPerPort"],
                    j["gpio"]["hasInterrupts"], j["gpio"]["hasPullUp"],
                    j["gpio"]["hasPullDown"]};
    profile.setGPIOConfig(gpio);

    UARTConfig uart{
        j["uart"]["numChannels"],
        j["uart"]["supportedBaudRates"].get<std::vector<uint32_t>>(),
        j["uart"]["hasFlowControl"], j["uart"]["txBufferSize"],
        j["uart"]["rxBufferSize"]};
    profile.setUARTConfig(uart);

    ADCConfig adc{j["adc"]["numChannels"], j["adc"]["resolution"],
                  j["adc"]["maxSampleRate"], j["adc"]["hasAutoTrigger"],
                  j["adc"]["hasDMA"]};
    profile.setADCConfig(adc);

    return profile;
  }

private:
  std::string name_;
  GPIOConfig gpioConfig_;
  UARTConfig uartConfig_;
  ADCConfig adcConfig_;
};

} // namespace ti_sdk