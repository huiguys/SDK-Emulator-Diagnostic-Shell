#pragma once

#include <deque>
#include <string>
#include <vector>


namespace ti_sdk {
namespace shell {

class HistoryManager {
public:
  HistoryManager(size_t maxSize = 1000);

  // Add a command to history
  void add(const std::string &command);

  // Get previous command
  std::string getPrevious();

  // Get next command
  std::string getNext();

  // Reset history navigation
  void resetNavigation();

  // Get all history entries
  const std::vector<std::string> &getAll() const;

  // Load history from file
  bool loadFromFile(const std::string &filename);

  // Save history to file
  bool saveToFile(const std::string &filename);

  // Clear history
  void clear();

private:
  std::deque<std::string> history_;
  size_t maxSize_;
  size_t currentIndex_;
};

} // namespace shell
} // namespace ti_sdk