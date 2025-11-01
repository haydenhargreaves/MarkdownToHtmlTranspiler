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
    program_name = argv[0];
    input_file = argv[1];

    // checks that file has correct file extension
    if (!(IsMarkupFile(input_file))) {
      throw std::invalid_argument(
          "Error: Invalid file extension. Expected a '.md' (Markdown) file.");
    }

    ParseArgs(argc, argv);
    

  } catch (const std::invalid_argument &e) {
      std::cerr << e.what() << "\n";
      PrintHelp(); 
      std::exit(EXIT_FAILURE);
  }
}

void CLI::ParseArgs(int argc, char** argv){
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-w" || arg == "--watch") {
            watchdog_enabled = true;
            continue;
        }

        if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            output_file = argv[++i];

            if (!(IsHtmlFile(output_file))) {
                throw std::invalid_argument("Error: Invalid file extension. Expected a '.html' (HTML) file.");
            }
            
            continue;
        }

        if (arg == "-o" || arg == "--output") {
            throw std::invalid_argument("Error: Missing filename after '-o' or '--output'.");
        }

        std::cerr << "Warning: Unrecognized argument '" << arg << "' ignored.\n";
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

bool CLI::IsHtmlFile(const std::string &filename) {
    // HTML file extension
    const std::string requiredExtension = ".html";

    // Ensure the filename is long enough to contain the extension
    if (filename.size() <= requiredExtension.size()) {
        return false;
    }

    // Check if the filename ends with ".html"
    return filename.substr(filename.size() - requiredExtension.size()) == requiredExtension;
}

void CLI::PrintHelp() const {
  std::cout << "Usage:\n"
            << "\t" << program_name << " <input_file>\n"
            << "Flags:\n"
            << "\t-o, --output <output_file>, optional output filename\n"
            << "\t-w, --watch, enables watchdog\n";
}

// ./parser input_file -w dhuahdakhk -o output
