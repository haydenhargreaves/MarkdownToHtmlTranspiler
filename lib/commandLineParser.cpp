#include "commandLineParser.h"
#include <iostream>
#include <stdexcept> // for std::invalid_argument

// implement hashmap for the code
CLI::CLI(int argc, char **argv) {
  try {
    if (argc < 2) {
      throw std::invalid_argument(
          "Error: No input file provided.\nUsage: <program> <input_file>");
    }
    // sets program info
    programName = argv[0];
    inputFile = argv[1];

    // checks that file has correct file extension
    if (!(CLI::IsMarkupFile(inputFile))) {
      throw std::invalid_argument(
          "Error: Invalid file extension. Expected a '.md' (Markdown) file.");
    }
    // stores remaining arguments
    for (int i = 1; i < argc; i++) {
      args.push_back(argv[i]);
    }
  } catch (const std::invalid_argument &e) {
    std::cerr << e.what() << "\n";
    PrintHelp();
    std::exit(EXIT_FAILURE);
  }
}

bool CLI::IsMarkupFile(const std::string &filename) {
  // markdown file extension
  const std::string requiredExtension = ".md";
  // checks to see if it can even include extension
  if (filename.size() <= requiredExtension.size()) {
    return false;
  }
  // Extracts the last N characters of the filename (where N is the length of
  // the required extension) and checks if they match the required file
  // extension (e.g., ".md").
  //
  // Example:
  //   filename = "notes.md"
  //   requiredExtension = ".md"
  //   filename.substr(filename.size() - requiredExtension.size()) == ".md" →
  //   true
  //
  // Reference: https://en.cppreference.com/w/cpp/string/basic_string/substr
  return filename.substr(filename.size() - requiredExtension.size()) ==
         requiredExtension;
}

// place to run commands but IDK IF WE SHOULD USE A HASMAP or how we are going
// to get each method to run
void CLI::RunCommands() {
  for (int i = 0; i < args.size(); i++) {
  }
}

void CLI::PrintHelp() const {
  std::cout << "Usage:\n"
            << " " << programName << " <input_file> REQUIRED\n"
            << "Flags:\n"
            << " --o, --output <outputFile>, optional output filename\n"
            << " --w, --watch, enables watchdog\n"
            << " --s, --stop, stops watchdog\n";
}
