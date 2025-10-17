#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>

using std::string;

class FileSystem {
public:
  FileSystem(string input_file_path, string output_file_path = "");

  /**
   * @brief Read the input file and return its content.
   *
   * This method will read the file at the input_file_path and create a single
   * output string. Each line will be delimited by either `\n` (Unix) or `\r\n`
   * (Windows). If the file path does not exist OR the file fails to open, this
   * method will throw a run time error.
   *
   * @return File contents as a single string.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  string ReadInputFile();

  /**
   * @brief Write the provided string to the output file.
   *
   * This method will attempt to open the output file and write the content
   * provided to the method in the file. If the file does not exist, it will be
   * created. If the file path does not exist OR the file fails to open, this
   * method will throw a run time error.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  void WriteOutputFile(string content);

protected:
  /**
   * @brief Input file path.
   *
   * Must be provided by the user.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  string input_file_path;

  /**
   * @brief Output file path.
   *
   * If not provided, will be generated using the `input_file_path` by removing
   * the extension and appending `.html`.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  string output_file_path;

private:
  /**
   * @brief Generate an output file path.
   *
   * If the user does not provide an output file path, this method can be
   * used to generate the path. This is done by simply swapping the `.md`
   * with `.html`.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  void GenerateOutputFilePath();
};

#endif
