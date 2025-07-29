#include "shell/history_manager.hpp"
#include <algorithm>
#include <fstream>


namespace ti_sdk {
namespace shell {

HistoryManager::HistoryManager(size_t maxSize)
    : maxSize_(maxSize), currentIndex_(0) {}

void HistoryManager::add(const std::string &command) {
  if (command.empty())
    return;

  // Don't add duplicate of most recent command
  if (!history_.empty() && history_.back() == command) {
    return;
  }

  history_.push_back(command);

  // Keep history size within limit
  while (history_.size() > maxSize_) {
    history_.pop_front();
  }

  resetNavigation();
}

std::string HistoryManager::getPrevious() {
  if (history_.empty() || currentIndex_ >= history_.size()) {
    return "";
  }

  return history_[history_.size() - 1 - currentIndex_++];
}

std::string HistoryManager::getNext() {
  if (currentIndex_ == 0) {
    return "";
  }

  return history_[history_.size() - currentIndex_--];
}

void HistoryManager::resetNavigation() { currentIndex_ = 0; }

const std::vector<std::string> &HistoryManager::getAll() const {
  static std::vector<std::string> vec;
  vec.assign(history_.begin(), history_.end());
  return vec;
}

bool HistoryManager::loadFromFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file)
    return false;

  clear();
  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty()) {
      add(line);
    }
  }

  return true;
}

bool HistoryManager::saveToFile(const std::string &filename) {
  std::ofstream file(filename);
  if (!file)
    return false;

  for (const auto &cmd : history_) {
    file << cmd << '\n';
  }

  return true;
}

void HistoryManager::clear() {
  history_.clear();
  resetNavigation();
}

} // namespace shell
} // namespace ti_sdk