#include "./parser.h"
#include <cctype>
#include <stdexcept>

using std::string;

void removeWhitespace(string &input) {
  size_t end = input.find_last_not_of(" \t\n\r\f\v");
  if (end != std::string::npos) {
    input.erase(end + 1);
  } else {
    input.clear(); // String contains only whitespace
  }

  size_t start = input.find_first_not_of(" \t\n\r\f\v");
  if (start != std::string::npos) {
    input.erase(0, start);
  } else {
    input.clear(); // String contains only whitespace
  }
}

Parser::Parser(string input_file_path, string output_file_path) {
  removeWhitespace(input_file_path);
  removeWhitespace(output_file_path);

  if (input_file_path == "") {
    throw std::runtime_error("input_file_path cannot be empty");
  }

  this->input_file_path = input_file_path;

  // NOTE: If the user does not provide an output file, then we should construct
  // one using the input file with .md swapped with the extension.
  if (output_file_path == "") {
    std::cout << "CLEANING" << std::endl;
    int ext_idx = input_file_path.find_last_of('.');
    string output_cleaned = input_file_path.substr(0, ext_idx) + ".html";
    this->output_file_path = output_cleaned;
  } else {
    this->output_file_path = output_file_path;
  }
}
