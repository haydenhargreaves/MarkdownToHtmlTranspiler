#include "parser.h"
#include "fileSystem.h"
#include "inlineNode.h"
#include "structureNode.h"
#include <algorithm>
#include <cctype>
#include <memory>
#include <string>

using std::string;
using std::vector;

void Parser::Inspect() {
  std::cerr << "Parser::Inspect() is not yet implemented." << std::endl;
}

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

void Parser::WriteOutput() {
  if (this->DOM == nullptr)
    throw std::runtime_error(
        "Cannot write output, output DOM tree does not exist. Please run the "
        "Parser::ParserDocument method first.");

  this->filesystem.WriteOutputFile(this->DOM->ToHtml());
}

void Parser::ParseDocument() {
  try {
    this->content = this->filesystem.ReadInputFile();
  } catch (const std::runtime_error &e) {
    std::cerr << "Caught an error: " << e.what() << std::endl;
    return;
  }

  // Remove the windows BS
  NormalizeInputStream();

  // We need document parent
  this->DOM = std::make_unique<DocumentNode>();

  while (!IsEOF()) {
    // std::cout << Peek(); Consume();
    auto block = ParseBlock();
    if (block != nullptr)
      this->DOM->AddChild(std::move(block));
  }
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

  if (node->IsEmpty())
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
      auto node = ParseBoldItalic();
      if (!node->IsEmpty())
        nodes.push_back(std::move(node));
      continue;
    } else if (c == '*' && Peek(1) == '*') {
      PushTextNode(nodes, str);
      auto node = ParseBold();
      if (!node->IsEmpty())
        nodes.push_back(std::move(node));
      continue;
    } else if (c == '*') {
      PushTextNode(nodes, str);
      auto node = ParseItalic();
      if (!node->IsEmpty())
        nodes.push_back(std::move(node));
      continue;
    }

    if (c == '`') {
      PushTextNode(nodes, str);
      auto node = ParseCode();
      if (!node->IsEmpty())
        nodes.push_back(std::move(node));
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

std::unique_ptr<Node> Parser::ParseCode() {
  string str;
  Consume(1);

  while (!IsEOF()) {
    char c = Peek();

    if (c == '\n' && Peek(1) == '\n')
      break;

    if (c == '`') {
      Consume(1);
      break;
    }

    str += c;
    Consume();
  }

  return std::make_unique<CodeNode>(str);
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
  char c = Peek();
  while (c == ' ' || c == '\t' || c == '\n') {
    Consume();
    c = Peek();
  }
}
