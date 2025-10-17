#ifndef PARSER_H
#define PARSER_H

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
  Parser(string input_file_path, string output_file_path = "");

  /**
   * @brief Inspect (view) contents of the class.
   *
   * Print each member of the class in its current state. Used for debugging.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  void Inspect();

  /**
   *
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

  // NOTE: We need a stack, just not sure what goes in it yet
  // std::stack<any> stack;

private:
  // windows... >:(
  void NormalizeInputStream();

  /**
   * @brief Parse a single block of content
   *
   * How does this function work...
   * This is where the magic happens.
   *
   * @return DOMNode, once exists
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  std::unique_ptr<Node> ParseBlock();

  // Stores index in the string
  size_t position = 0;

  // Working input content
  string content;

  std::unique_ptr<Node> ParseParagraph();
  std::unique_ptr<Node> ParseHeading();
  vector<std::unique_ptr<Node>> ParseInline();

  void PushTextNode(vector<std::unique_ptr<Node>> &nodes, string &str);

  std::unique_ptr<Node> ParseItalic();
  std::unique_ptr<Node> ParseBold();
  std::unique_ptr<Node> ParseBoldItalic();

  char Peek(size_t offset = 0);
  void Consume(size_t count = 1);
  bool IsEOF();

  void ConsumeWhiteSpace();
};

#endif
