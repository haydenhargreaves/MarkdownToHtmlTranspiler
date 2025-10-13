#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>

class Parser {
private:
  std::string input_file_path;
  std::string output_file_path;

public:
  Parser(std::string input_file_path, std::string output_file_path = "");

  inline void Print() {
    std::cout << this->input_file_path << " -> " << this->output_file_path
              << std::endl;
  }
};

#endif
