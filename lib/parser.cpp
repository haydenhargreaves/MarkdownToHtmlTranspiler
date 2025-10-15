#include "./parser.h"
#include "./util.h"
#include <cctype>
#include <stdexcept>

using std::string;

Parser::Parser(string input_file_path, string output_file_path) {
  // NOTE: Remove any white space AROUND the inputs
  removeWhitespace(input_file_path);
  removeWhitespace(output_file_path);

  if (input_file_path == "") {
    throw std::runtime_error("input_file_path cannot be empty");
  }

  this->input_file_path = input_file_path;

  // NOTE: If the user does not provide an output file, then we should construct
  // one using the input file with .md swapped with the extension.
  if (output_file_path == "") {
    int ext_idx = input_file_path.find_last_of('.');
    string output_cleaned = input_file_path.substr(0, ext_idx) + ".html";
    this->output_file_path = output_cleaned;
    return;
  }

  this->output_file_path = output_file_path;
}

void Parser::Inspect() {
  std::cout << "std::string input_file_path: " << this->input_file_path
            << std::endl;
  std::cout << "std::string output_file_path: " << this->output_file_path
            << std::endl;
}
