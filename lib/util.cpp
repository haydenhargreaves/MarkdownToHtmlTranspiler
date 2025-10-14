#include "./util.h"

void removeTrailingWhitespace(std::string &input) {
  size_t start = input.find_first_not_of(" \t\n\r\f\v");
  if (start != std::string::npos) {
    input.erase(0, start);
  } else {
    input.clear();
  }
}

void removeLeadingWhitespace(std::string &input) {
  size_t end = input.find_last_not_of(" \t\n\r\f\v");
  if (end != std::string::npos) {
    input.erase(end + 1);
  } else {
    input.clear();
  }
}

void removeWhitespace(std::string &input) {
  removeLeadingWhitespace(input);
  removeTrailingWhitespace(input);
}
