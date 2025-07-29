#include "shell/cli_manager.hpp"
#include <algorithm>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <windows.h>


namespace ti_sdk {
namespace shell {

CLIManager::CLIManager() : running_(false), history_index_(0), cursor_pos_(0) {
  // Register built-in commands
  registerCommand("help", "Display available commands",
                  [this](const auto &args) { return helpCommand(args); });

  registerCommand("exit", "Exit the shell",
                  [this](const auto &args) { return exitCommand(args); });

  registerCommand("history", "Show command history",
                  [this](const auto &args) { return historyCommand(args); });
}

CLIManager::~CLIManager() {}

void CLIManager::run() {
  running_ = true;
  std::cout << "TI SDK Emulator Shell v1.0\n"
            << "Type 'help' for available commands\n\n";

  while (running_) {
    std::cout << "ti-sdk> ";
    std::string input = readLine();
    if (!input.empty()) {
      addToHistory(input);
      executeCommand(input);
    }
  }
}

std::string CLIManager::readLine() {
  current_line_.clear();
  cursor_pos_ = 0;

  while (true) {
    if (_kbhit()) {
      int ch = _getch();

      if (ch == 224 || ch == 0) { // Special keys
        ch = _getch();
        switch (ch) {
        case 72: // Up arrow
          handleUpArrow();
          break;
        case 80: // Down arrow
          handleDownArrow();
          break;
        case 75: // Left arrow
          if (cursor_pos_ > 0) {
            --cursor_pos_;
            std::cout << "\b";
          }
          break;
        case 77: // Right arrow
          if (cursor_pos_ < current_line_.length()) {
            ++cursor_pos_;
            std::cout << current_line_[cursor_pos_ - 1];
          }
          break;
        }
      } else if (ch == '\r' || ch == '\n') { // Enter
        std::cout << "\n";
        return current_line_;
      } else if (ch == '\b' || ch == 127) { // Backspace
        handleBackspace();
      } else if (ch == '\t') { // Tab
        handleTab(current_line_);
      } else if (ch >= 32 && ch <= 126) { // Printable characters
        if (cursor_pos_ == current_line_.length()) {
          current_line_ += static_cast<char>(ch);
          ++cursor_pos_;
          std::cout << static_cast<char>(ch);
        } else {
          current_line_.insert(cursor_pos_, 1, static_cast<char>(ch));
          ++cursor_pos_;
          // Redraw the line from cursor position
          std::cout << static_cast<char>(ch);
          std::cout << current_line_.substr(cursor_pos_);
          // Move cursor back to position
          for (size_t i = cursor_pos_; i < current_line_.length(); ++i) {
            std::cout << "\b";
          }
        }
      }
    }
  }
}

void CLIManager::handleBackspace() {
  if (cursor_pos_ > 0) {
    if (cursor_pos_ == current_line_.length()) {
      current_line_.pop_back();
      --cursor_pos_;
      std::cout << "\b \b";
    } else {
      current_line_.erase(cursor_pos_ - 1, 1);
      --cursor_pos_;
      std::cout << "\b";
      // Redraw the line from cursor position
      std::cout << current_line_.substr(cursor_pos_) << " ";
      // Move cursor back to position
      for (size_t i = cursor_pos_; i <= current_line_.length(); ++i) {
        std::cout << "\b";
      }
    }
  }
}

void CLIManager::handleUpArrow() {
  if (!history_.empty() && history_index_ < history_.size()) {
    // Clear current line
    for (size_t i = 0; i < current_line_.length(); ++i) {
      std::cout << "\b \b";
    }

    // Show previous command
    ++history_index_;
    current_line_ = history_[history_.size() - history_index_];
    cursor_pos_ = current_line_.length();
    std::cout << current_line_;
  }
}

void CLIManager::handleDownArrow() {
  if (history_index_ > 0) {
    // Clear current line
    for (size_t i = 0; i < current_line_.length(); ++i) {
      std::cout << "\b \b";
    }

    // Show next command
    --history_index_;
    if (history_index_ == 0) {
      current_line_.clear();
    } else {
      current_line_ = history_[history_.size() - history_index_];
    }
    cursor_pos_ = current_line_.length();
    std::cout << current_line_;
  }
}

void CLIManager::handleTab(const std::string &current) {
  std::vector<std::string> matches;

  // Find matching commands
  for (const auto &[cmd, _] : commands_) {
    if (cmd.substr(0, current.length()) == current) {
      matches.push_back(cmd);
    }
  }

  if (matches.empty()) {
    return;
  }

  if (matches.size() == 1) {
    // Clear current line
    for (size_t i = 0; i < current_line_.length(); ++i) {
      std::cout << "\b \b";
    }

    // Complete the command
    current_line_ = matches[0];
    cursor_pos_ = current_line_.length();
    std::cout << current_line_;
  } else {
    // Show all possible completions
    std::cout << "\n";
    for (const auto &match : matches) {
      std::cout << match << "  ";
    }
    std::cout << "\nti-sdk> " << current_line_;
  }
}

void CLIManager::registerCommand(const std::string &command,
                                 const std::string &help,
                                 CommandCallback callback) {
  commands_[command] = Command{help, callback};
}

void CLIManager::addToHistory(const std::string &command) {
  if (history_.size() >= MAX_HISTORY) {
    history_.erase(history_.begin());
  }
  history_.push_back(command);
  history_index_ = 0;
}

std::vector<std::string> CLIManager::getHistory() const { return history_; }

bool CLIManager::loadHistory(const std::string &filename) {
  std::ifstream file(filename);
  if (!file)
    return false;

  history_.clear();
  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty()) {
      history_.push_back(line);
    }
  }

  return true;
}

bool CLIManager::saveHistory(const std::string &filename) {
  std::ofstream file(filename);
  if (!file)
    return false;

  for (const auto &cmd : history_) {
    file << cmd << '\n';
  }

  return true;
}

bool CLIManager::executeCommand(const std::string &input) {
  std::istringstream iss(input);
  std::vector<std::string> tokens;
  std::string token;

  while (iss >> token) {
    tokens.push_back(token);
  }

  if (tokens.empty())
    return true;

  std::string command = tokens[0];
  std::vector<std::string> args(tokens.begin() + 1, tokens.end());

  auto it = commands_.find(command);
  if (it == commands_.end()) {
    std::cout << "Unknown command: " << command << "\n"
              << "Type 'help' for available commands\n";
    return false;
  }

  return it->second.callback(args);
}

bool CLIManager::helpCommand(const std::vector<std::string> &args) {
  if (!args.empty()) {
    auto it = commands_.find(args[0]);
    if (it != commands_.end()) {
      std::cout << args[0] << ": " << it->second.help << "\n";
      return true;
    }
    std::cout << "No help available for: " << args[0] << "\n";
    return false;
  }

  std::cout << "Available commands:\n";
  for (const auto &[cmd, info] : commands_) {
    std::cout << "  " << cmd << " - " << info.help << "\n";
  }
  return true;
}

bool CLIManager::exitCommand(const std::vector<std::string> &args) {
  running_ = false;
  return true;
}

bool CLIManager::historyCommand(const std::vector<std::string> &args) {
  int index = 1;
  for (const auto &cmd : history_) {
    std::cout << " " << index++ << "  " << cmd << "\n";
  }
  return true;
}

} // namespace shell
} // namespace ti_sdk