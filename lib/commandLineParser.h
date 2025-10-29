/**
 * @file commandLineParser.h
 * @brief Parses command line
 * @author Preston Shultz
 * Sources:
 *
 */

#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string>
#include <vector>

/**
 * @brief CommandLine Helper class
 *
 * Parse input arguments, sets input and output files
 * Lets user use commands with program
 *
 * @author Preston Shultz (shultzp1@my.erau.edu)
 */
class CLI {
public:
  /**
   * @brief Takes in argc and argv and converts it to vector
   *
   * CLI constructor
   *
   * @author Preston Shultz (shultzp1@my.erau.edu)
   */
  CLI(int argc, char **argv);

  /**
   * @brief Runs Commands
   *
   * @author Preston Shultz (shultzp1@my.erau.edu)
   */
  void RunCommands();

  /**
   * @brief Prints a list of commands that can be used
   *
   * @author Preston Shultz (shultzp1@my.erau.edu)
   */
  void PrintHelp() const;

  /**
   * @brief Returns input file
   *
   * @author Preston Shultz (shultzp1@my.erau.edu)
   */
  std::string GetInputFile() const { return input_file; }

  /**
   * @brief Returns output file
   *
   * @author Preston Shultz (shultzp1@my.erau.edu)
   */
  std::string GetOutputFile() const { return output_file; }

private:
  /**
   * @brief Ensures the provided filename has a .markup extension
   *
   * @param filename The file name to validate
   * @return true if file ends with .markup
   * @return false otherwise
   * @author Preston Shultz (shultzp1@my.erau.edu)
   */
  static bool IsMarkupFile(const std::string &filename);

  std::string program_name;
  std::vector<std::string> args;
  std::string input_file;
  std::string output_file;
};

#endif
