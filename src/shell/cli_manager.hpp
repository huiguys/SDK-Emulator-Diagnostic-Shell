#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


namespace ti_sdk {
namespace shell {

class CLIManager {
public:
  using CommandCallback = std::function<bool(const std::vector<std::string> &)>;

  CLIManager();
  ~CLIManager();

  // Start the CLI loop
  void run();

  // Register a new command
  void registerCommand(const std::string &command, const std::string &help,
                       CommandCallback callback);

  // Add command to history
  void addToHistory(const std::string &command);

  // Get command history
  std::vector<std::string> getHistory() const;

  // Load command history from file
  bool loadHistory(const std::string &filename);

  // Save command history to file
  bool saveHistory(const std::string &filename);

private:
  struct Command {
    std::string help;
    CommandCallback callback;
  };

  // Parse and execute a command
  bool executeCommand(const std::string &input);

  // Built-in commands
  bool helpCommand(const std::vector<std::string> &args);
  bool exitCommand(const std::vector<std::string> &args);
  bool historyCommand(const std::vector<std::string> &args);

  // Windows-specific input handling
  std::string readLine();
  void handleBackspace();
  void handleUpArrow();
  void handleDownArrow();
  void handleTab(const std::string &current);

  // Command registry
  std::unordered_map<std::string, Command> commands_;

  // Command history
  std::vector<std::string> history_;
  size_t history_index_;
  static constexpr size_t MAX_HISTORY = 1000;

  // Current input buffer
  std::string current_line_;
  size_t cursor_pos_;

  // Running state
  bool running_;
};

} // namespace shell
} // namespace ti_sdk