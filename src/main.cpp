#include "sdk/adc.hpp"
#include "sdk/gpio.hpp"
#include "sdk/uart.hpp"
#include "shell/cli_manager.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>


using namespace ti_sdk;

// Helper function to parse pin arguments
bool parsePinArgs(const std::vector<std::string> &args, uint8_t &port,
                  uint8_t &pin) {
  if (args.size() < 2) {
    std::cout << "Error: Missing port and pin arguments\n";
    return false;
  }

  try {
    port = std::stoi(args[0]);
    pin = std::stoi(args[1]);
    return true;
  } catch (const std::exception &) {
    std::cout << "Error: Invalid port or pin number\n";
    return false;
  }
}

int main() {
  // Initialize subsystems
  if (!GPIO::initialize()) {
    std::cerr << "Failed to initialize GPIO subsystem\n";
    return 1;
  }

  if (!UART::initialize(115200)) {
    std::cerr << "Failed to initialize UART subsystem\n";
    return 1;
  }

  if (!ADC::initialize()) {
    std::cerr << "Failed to initialize ADC subsystem\n";
    return 1;
  }

  shell::CLIManager cli;

  // Register GPIO commands
  cli.registerCommand(
      "gpio-config", "Configure a GPIO pin: gpio-config <port> <pin> <mode>",
      [](const auto &args) {
        uint8_t port, pin;
        if (!parsePinArgs(args, port, pin))
          return false;

        if (args.size() < 3) {
          std::cout << "Error: Missing mode argument\n";
          return false;
        }

        PinMode mode;
        const auto &modeStr = args[2];
        if (modeStr == "input")
          mode = PinMode::INPUT;
        else if (modeStr == "output")
          mode = PinMode::OUTPUT;
        else if (modeStr == "input-pullup")
          mode = PinMode::INPUT_PULLUP;
        else if (modeStr == "input-pulldown")
          mode = PinMode::INPUT_PULLDOWN;
        else {
          std::cout << "Error: Invalid mode. Valid modes are: input, output, "
                       "input-pullup, input-pulldown\n";
          return false;
        }

        if (!GPIO::configurePin(port, pin, mode)) {
          std::cout << "Error: Failed to configure pin\n";
          return false;
        }

        std::cout << "Pin configured successfully\n";
        return true;
      });

  cli.registerCommand(
      "gpio-write", "Write to a GPIO pin: gpio-write <port> <pin> <value>",
      [](const auto &args) {
        uint8_t port, pin;
        if (!parsePinArgs(args, port, pin))
          return false;

        if (args.size() < 3) {
          std::cout << "Error: Missing value argument\n";
          return false;
        }

        PinState state;
        const auto &valueStr = args[2];
        if (valueStr == "0" || valueStr == "low")
          state = PinState::LOW;
        else if (valueStr == "1" || valueStr == "high")
          state = PinState::HIGH;
        else {
          std::cout << "Error: Invalid value. Use 0/low or 1/high\n";
          return false;
        }

        if (!GPIO::writePin(port, pin, state)) {
          std::cout << "Error: Failed to write to pin\n";
          return false;
        }

        std::cout << "Value written successfully\n";
        return true;
      });

  cli.registerCommand(
      "gpio-read", "Read from a GPIO pin: gpio-read <port> <pin>",
      [](const auto &args) {
        uint8_t port, pin;
        if (!parsePinArgs(args, port, pin))
          return false;

        auto state = GPIO::readPin(port, pin);
        std::cout << "Pin value: " << (state == PinState::HIGH ? "HIGH" : "LOW")
                  << "\n";
        return true;
      });

  // Register ADC commands
  cli.registerCommand(
      "adc-config",
      "Configure an ADC channel: adc-config <channel> <sample-rate>",
      [](const auto &args) {
        if (args.size() < 2) {
          std::cout << "Error: Missing channel and sample rate arguments\n";
          return false;
        }

        try {
          uint8_t channel = std::stoi(args[0]);
          uint32_t sampleRate = std::stoi(args[1]);

          if (!ADC::configureChannel(channel, sampleRate)) {
            std::cout << "Error: Failed to configure ADC channel\n";
            return false;
          }

          std::cout << "ADC channel configured successfully\n";
          return true;
        } catch (const std::exception &) {
          std::cout << "Error: Invalid channel or sample rate\n";
          return false;
        }
      });

  cli.registerCommand("adc-read",
                      "Read from an ADC channel: adc-read <channel> [samples]",
                      [](const auto &args) {
                        if (args.empty()) {
                          std::cout << "Error: Missing channel argument\n";
                          return false;
                        }

                        try {
                          uint8_t channel = std::stoi(args[0]);
                          uint16_t value;

                          if (args.size() > 1) {
                            uint8_t samples = std::stoi(args[1]);
                            value = ADC::readAverage(channel, samples);
                            std::cout << "Average ADC value: " << value << "\n";
                          } else {
                            value = ADC::read(channel);
                            std::cout << "ADC value: " << value << "\n";
                          }
                          return true;
                        } catch (const std::exception &) {
                          std::cout << "Error: Invalid channel or samples\n";
                          return false;
                        }
                      });

  // Start the CLI
  cli.run();

  return 0;
}