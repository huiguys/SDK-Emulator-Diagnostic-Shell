#pragma once

#include <string>
#include <vector>

namespace ti_sdk {
namespace shell {

class CommandParser {
public:
  // Parse a command line into tokens
  static std::vector<std::string> parse(const std::string &input);

  // Check if a string is a valid command name
  static bool isValidCommandName(const std::string &name);

  // Get command name from input
  static std::string getCommandName(const std::string &input);

  // Get command arguments from input
  static std::vector<std::string> getArguments(const std::string &input);

private:
  CommandParser() = delete; // Prevent instantiation

  // Helper functions
  static bool isWhitespace(char c);
  static bool isQuote(char c);
  static bool isEscapeChar(char c);
};

} // namespace shell
} // namespace ti_sdk