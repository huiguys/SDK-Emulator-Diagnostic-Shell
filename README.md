# TI SDK Emulator & Diagnostic Shell

A command-line tool for simulating and testing TI SDK behavior, particularly focusing on GPIO, UART, and ADC functionality.

## Features

- Simulated GPIO APIs with full pin control
- Interactive shell interface with command history
- State persistence (save/restore peripheral states)
- Extensible command system
- Built-in help system

## Building

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- readline library
- nlohmann-json library

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

Run the emulator:

```bash
./ti_sdk_shell
```

### Available Commands

- GPIO Configuration:
  ```
  gpio-config <port> <pin> <mode>
  ```
  Modes: input, output, input-pullup, input-pulldown

- GPIO Operations:
  ```
  gpio-write <port> <pin> <value>    # Write 0 or 1
  gpio-read <port> <pin>             # Read current value
  gpio-toggle <port> <pin>           # Toggle pin state
  ```

- State Management:
  ```
  save-state <filename>    # Save current peripheral state
  load-state <filename>    # Restore peripheral state
  ```

- Shell Commands:
  ```
  help                    # Show available commands
  history                 # Show command history
  exit                    # Exit the shell
  ```

## Examples

1. Configure a pin as output:
   ```
   ti-sdk> gpio-config 1 0 output
   ```

2. Write a value:
   ```
   ti-sdk> gpio-write 1 0 1
   ```

3. Read a pin value:
   ```
   ti-sdk> gpio-read 1 0
   ```

4. Save current state:
   ```
   ti-sdk> save-state gpio_state.json
   ```

## Project Structure

```
.
├── CMakeLists.txt           # Main CMake configuration
├── src/
│   ├── main.cpp            # Entry point
│   ├── sdk/                # SDK implementation
│   │   ├── gpio.hpp
│   │   ├── gpio.cpp
│   │   └── ...
│   └── shell/             # CLI implementation
│       ├── cli_manager.hpp
│       ├── cli_manager.cpp
│       └── ...
└── tests/                 # Unit tests
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

MIT License - see LICENSE file for details 