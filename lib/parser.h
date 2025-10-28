#ifndef PARSER_H
#define PARSER_H

#include "fileSystem.h"
#include "node.h"
#include <iostream>
#include <memory>
#include <stack>
#include <string>

using std::string;
using std::vector;

/**
 * @brief Markdown parser class.
 *
 * Converts a Markdown file into an HTML output. This is done using a
 * recursive descent parser and converting the Markdown into a DOM tree.
 * Once the DOM tree exists, it is converted into an HTML string and
 * written to the output file provided.
 *
 * This class will have a `DOM` and a `DOMParser` which are used in this
 * process.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class Parser {
public:
  Parser(string input_file_path, string output_file_path = "")
      : filesystem(input_file_path, output_file_path) {};

  /**
   * @brief Inspect (view) contents of the class.
   *
   * Print each member of the class in its current state. Used for debugging.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  void Inspect();

  /**
   * @brief Parse an entire document.
   *
   * This function will be called to yield the result. This is the entry point
   * to the recursive descent parser.
   *
   * Currently, there are no parameters, they are still under consideration.
   *
   * It will be important to remember states between lines. For example, a
   * paragraph that spans many lines should be inside the same node. But
   * white space causes the node to be broken.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  void ParseDocument();

  /**
   * @brief Write the output to the file.
   *
   * Once the tree is generated, this method should be called to actually
   * write the output. Having this functionality separate allows for more
   * portability.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  void WriteOutput();

protected:
  /**
   * @brief File system module to handle file I/O.
   *
   * Anything requiring file I/O operations will be handled by this module.
   *
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  FileSystem filesystem;

  /**
   * @brief Parser generated tree.
   *
   * This value will store the root, which is expected to be a DocumentNode.
   * This node will mark the start of the tree. The parser will populate this
   * tree during the parsing process.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  std::unique_ptr<Node> DOM;

private:
  /**
   * @brief Normalize the input stream.
   *
   * Replaces all `\r\n` with just `\n` since that is what the parser expects.
   * Then removes any left over `\r` elements in the stream. If the stream is
   * empty this method does nothing.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  void NormalizeInputStream();

  /**
   * @brief Parse a single block of content
   *
   * How does this function work...
   * This is where the magic happens.
   *
   * @return Node, to be appended to the callers children.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  std::unique_ptr<Node> ParseBlock();

  // Stores index in the string
  size_t position = 0;

  // Working input content
  string content;

  // TODO: Document these methods, no more magic methods :)

  std::unique_ptr<Node> ParseParagraph();
  std::unique_ptr<Node> ParseHeading();
  std::unique_ptr<Node> ParseList(bool ordered);
  vector<std::unique_ptr<Node>> ParseInline();

  // The only differences are the exit condition
  vector<std::unique_ptr<Node>> ParseInlineHeading();
  vector<std::unique_ptr<Node>> ParseInlineListContent();

  void PushTextNode(vector<std::unique_ptr<Node>> &nodes, string &str);

  std::unique_ptr<Node> ParseItalic();
  std::unique_ptr<Node> ParseBold();
  std::unique_ptr<Node> ParseBoldItalic();
  std::unique_ptr<Node> ParseCode();

  char Peek(size_t offset = 0);
  void Consume(size_t count = 1);
  bool IsEOF();

  void ConsumeWhiteSpace();
};

#endif
