#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <stack>
#include <string>

using std::string;

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
  void ParseDocument(void);

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

  // NOTE: We need a stack, just not sure what goes in it yet
  // std::stack<any> stack;

private:
  /**
   * @brief Parse a single line.
   *
   * How does this function work...
   * This is where the magic happens.
   *
   * @param line Target line to parse, as string.
   * @return DOMNode, once exists
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  void ParseLine(string line);

  // NOTE: Parser operations, again, abstract, just for brainstorming now
  //       These should operate on internal state, not lines themselves
  void ParseHeader();
  void ParseParagraph();
  void ParseItalic();
  void ParseBold();
  void ParseBoldItalic();

  // NOTE: Character operations, these are just for brainstorming
  char Peek();
  void Consume();
  bool EndOfLine();
};

#endif
