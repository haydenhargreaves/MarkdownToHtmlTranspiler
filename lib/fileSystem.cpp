#include "fileSystem.h"
#include "util.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

FileSystem::FileSystem(string input_file_path, string output_file_path) {
  removeWhitespace(input_file_path);
  removeWhitespace(output_file_path);

  if (input_file_path.empty())
    throw std::runtime_error("input_file_path cannot be empty");

  this->input_file_path = input_file_path;

  this->output_file_path = output_file_path;
  if (this->output_file_path.empty())
    GenerateOutputFilePath();
};

void FileSystem::GenerateOutputFilePath() {
  if (this->input_file_path.empty())
    throw std::runtime_error("Cannot generate output path from empty input.");

  int ext_idx = this->input_file_path.find_last_of('.');
  string output_cleaned = this->input_file_path.substr(0, ext_idx) + ".html";
  this->output_file_path = output_cleaned;
}

std::string FileSystem::ReadInputFile() {
  // Cannot read file if the path does not exist
  if (this->input_file_path.empty())
    throw std::runtime_error("Cannot open file: path was not provided.");

  std::ifstream input_file(this->input_file_path);

  if (!input_file.is_open())
    throw std::runtime_error("Failed to open input file.");

  // Read the file into a single string using a string stream
  std::stringstream buffer;
  buffer << input_file.rdbuf();
  input_file.close();

  return buffer.str();
}

void FileSystem::WriteOutputFile(std::string content) {
  // Cannot write to file if the path does not exist
  if (this->output_file_path.empty())
    throw std::runtime_error("Cannot open file: path was not provided.");

  std::ofstream output_file(this->output_file_path);

  if (!output_file.is_open())
    throw std::runtime_error("Failed to open output file.");

  output_file << content;
  output_file.close();
}
