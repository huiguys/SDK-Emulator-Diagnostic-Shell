#include "shell/command_parser.hpp"
#include <cctype>
#include <sstream>

namespace ti_sdk {
namespace shell {

std::vector<std::string> CommandParser::parse(const std::string &input) {
  std::vector<std::string> tokens;
  std::string token;
  bool inQuotes = false;
  bool escaped = false;

  for (size_t i = 0; i < input.length(); ++i) {
    char c = input[i];

    if (escaped) {
      token += c;
      escaped = false;
      continue;
    }

    if (isEscapeChar(c)) {
      escaped = true;
      continue;
    }

    if (isQuote(c)) {
      inQuotes = !inQuotes;
      continue;
    }

    if (!inQuotes && isWhitespace(c)) {
      if (!token.empty()) {
        tokens.push_back(token);
        token.clear();
      }
      continue;
    }

    token += c;
  }

  if (!token.empty()) {
    tokens.push_back(token);
  }

  return tokens;
}

bool CommandParser::isValidCommandName(const std::string &name) {
  if (name.empty())
    return false;

  // First character must be a letter or underscore
  if (!std::isalpha(name[0]) && name[0] != '_')
    return false;

  // Rest can be alphanumeric or underscore or hyphen
  for (size_t i = 1; i < name.length(); ++i) {
    char c = name[i];
    if (!std::isalnum(c) && c != '_' && c != '-')
      return false;
  }

  return true;
}

std::string CommandParser::getCommandName(const std::string &input) {
  auto tokens = parse(input);
  return tokens.empty() ? "" : tokens[0];
}

std::vector<std::string> CommandParser::getArguments(const std::string &input) {
  auto tokens = parse(input);
  return tokens.size() > 1
             ? std::vector<std::string>(tokens.begin() + 1, tokens.end())
             : std::vector<std::string>();
}

bool CommandParser::isWhitespace(char c) { return std::isspace(c); }

bool CommandParser::isQuote(char c) { return c == '"' || c == '\''; }

bool CommandParser::isEscapeChar(char c) { return c == '\\'; }

} // namespace shell
} // namespace ti_sdk