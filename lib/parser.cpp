#include "parser.h"
#include "inlineNode.h"
#include "structureNode.h"
#include "util.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

using std::string;
using std::vector;

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

// replace '\r\n' with '\n'
void Parser::NormalizeInputStream() {
  if (this->content.empty())
    return;

  size_t pos = 0;
  while ((pos = content.find("\r\n", pos)) != string::npos) {
    this->content.replace(pos, 2, "\n");
    pos++;
  }

  // NOTE: Remove all occurrences of '\r'
  this->content.erase(
      std::remove(this->content.begin(), this->content.end(), '\r'),
      this->content.end());
}

void Parser::ParseDocument() {
  // Open the input file
  std::ifstream input_file(this->input_file_path);

  if (!input_file.is_open()) {
    throw std::runtime_error("Failed to open input file.");
    return;
  }

  // Read the file into a single string
  std::stringstream buffer;
  buffer << input_file.rdbuf();
  this->content = buffer.str();

  input_file.close();

  // We need document parent
  this->DOM = std::make_unique<DocumentNode>();

  while (!IsEOF()) {
    // std::cout << Peek(); Consume();
    auto block = ParseBlock();
    if (block != nullptr)
      this->DOM->AddChild(std::move(block));
  }

  std::cout << this->DOM->ToHtml();
}

// All this does is pick which subparser to call
// Identify which block to parse
std::unique_ptr<Node> Parser::ParseBlock() {
  // Remove whitespace using peek and consume (' ', '\t', '\n')
  ConsumeWhiteSpace();

  // NOTE: Simple example
  // std::string ch(1, Peek());
  // std::unique_ptr<Node> block = std::make_unique<TextNode>(ch);
  // Consume();

  if (Peek() == '#') {
    return ParseHeading();
  }

  // this is the default case
  return ParseParagraph();
}

std::unique_ptr<Node> Parser::ParseParagraph() {
  auto node = std::make_unique<ParagraphNode>();

  // This should call parse inline
  auto text_nodes = ParseInline();
  for (auto &text_node : text_nodes) {
    node->AddChild(std::move(text_node));
  }

  if (node->GetChilren().size() < 1)
    return nullptr;

  return node;
}

std::unique_ptr<Node> Parser::ParseHeading() {
  // Compute the size of the heading
  int i = 0;
  char c = Peek();
  while (c == '#') {
    c = Peek(i++);
  }

  Consume(i - 1);
  auto node = std::make_unique<HeadingNode>(i - 1);

  ConsumeWhiteSpace();

  std::string str;
  while (!IsEOF()) {
    c = Peek();
    // We can stop as soon as we see a new line. Headings are single line blocks
    if (c == '\n')
      break;

    // If a newline, use a space instead
    str += c;
    Consume();
  }

  // BUG: Why do we need to check this?
  if (str == "")
    return nullptr;

  auto text_node = std::make_unique<TextNode>(str);
  node->AddChild(std::move(text_node));

  return node;
}

vector<std::unique_ptr<Node>> Parser::ParseInline() {
  vector<std::unique_ptr<Node>> nodes;
  string str;

  while (!IsEOF()) {
    char c = Peek();
    // If this char and next char are both newlines: then we have an empty line,
    // we should stop.
    if (c == '\n' && Peek(1) == '\n')
      break;

    if (c == '*' && Peek(1) == '*' && Peek(2) == '*') {
      PushTextNode(nodes, str);
      nodes.push_back(std::move(ParseBoldItalic()));
      continue;
    } else if (c == '*' && Peek(1) == '*') {
      PushTextNode(nodes, str);
      nodes.push_back(std::move(ParseBold()));
      continue;
    } else if (c == '*') {
      PushTextNode(nodes, str);
      nodes.push_back(std::move(ParseItalic()));
      continue;
    }

    // If a newline, use a space instead
    str += (c == '\n' ? ' ' : c);
    Consume();
  }

  // Push the last node, if the string is not empty
  PushTextNode(nodes, str);
  return nodes;
}

std::unique_ptr<Node> Parser::ParseItalic() {
  string str;
  Consume(1);

  while (!IsEOF()) {
    char c = Peek();

    if (c == '\n' && Peek(1) == '\n')
      break;

    if (c == '*') {
      Consume(1);
      break;
    }

    str += c;
    Consume();
  }

  return std::make_unique<ItalicNode>(str);
}

std::unique_ptr<Node> Parser::ParseBold() {
  string str;
  Consume(2);

  while (!IsEOF()) {
    char c = Peek();

    if (c == '\n' && Peek(1) == '\n')
      break;

    if (c == '*' && Peek(1) == '*') {
      Consume(2);
      break;
    }

    str += c;
    Consume();
  }

  return std::make_unique<BoldNode>(str);
}

std::unique_ptr<Node> Parser::ParseBoldItalic() {
  string str;
  Consume(3);

  while (!IsEOF()) {
    char c = Peek();

    if (c == '\n' && Peek(1) == '\n')
      break;

    if (c == '*' && Peek(1) == '*' && Peek(2) == '*') {
      Consume(3);
      break;
    }

    str += c;
    Consume();
  }

  return std::make_unique<BoldItalicNode>(str);
}

void Parser::PushTextNode(vector<std::unique_ptr<Node>> &nodes, string &str) {
  if (!str.empty())
    nodes.push_back(std::move(std::make_unique<TextNode>(str)));
  str = "";
}

char Parser::Peek(size_t offset) {
  size_t look_ahead_pos = this->position + offset;

  if (look_ahead_pos < this->content.length()) {
    return this->content[look_ahead_pos];
  }

  return '\0'; // null if past end
};

void Parser::Consume(size_t count) { this->position += count; };

bool Parser::IsEOF() { return this->position >= this->content.length(); };

void Parser::ConsumeWhiteSpace() {
  // TODO: This can be optimized using an accumulator and then consuming
  char c = Peek();
  while (c == ' ' || c == '\t' || c == '\n') {
    Consume();
    c = Peek();
  }
}
