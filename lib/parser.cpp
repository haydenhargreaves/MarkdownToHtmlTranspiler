#include "parser.h"
#include "fileSystem.h"
#include "inlineNode.h"
#include "structureNode.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <string>

using std::string;
using std::vector;

void Parser::Inspect() {
  std::cout << this->position << std::endl;
  std::cout << this->content.size() << std::endl;
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
  // NOTE:This needs to be set so the parsing can continue
  this->position = 0;

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

  char c = Peek();
  char c_next = Peek(1);

  // 1. Parse heading
  if (c == '#') {
    return ParseHeading();
  }

  // 2. Parser unordered list
  if (c == '*' || c == '-' || c == '+') {
    // Next character must be space or tab
    if (c_next == ' ' || c_next == '\t') {
      return ParseList(false);
    }
  }

  // 3. Parse ordered list
  if (std::isdigit(c) && c_next == '.') {
    return ParseList(true);
  }

  // 4. Parse code block
  if (c == '`' && c_next == '`' && Peek(2) == '`') {
    return ParseCodeBlock();
  }

  // 5. Parse image
  if (c == '!' && c_next == '[') {
    return ParseImage();
  }

  // 6. Parser paragraph
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

  // This should call parse inline
  auto text_nodes = ParseInlineHeading();
  for (auto &text_node : text_nodes) {
    node->AddChild(std::move(text_node));
  }

  if (node->IsEmpty())
    return nullptr;

  return node;
}

std::unique_ptr<Node> Parser::ParseList(bool ordered) {
  auto node = std::make_unique<ListNode>(ordered);

  // Consume the required white space and list char ('* ' or '1.')
  while (true) {

    Consume(ordered ? 2 : 1);
    ConsumeWhiteSpace();

    // std::unique_ptr<Node> Parser::ParseParagraph() {
    //   auto node = std::make_unique<ParagraphNode>();
    //
    //   // This should call parse inline
    //   auto text_nodes = ParseInline();
    //   for (auto &text_node : text_nodes) {
    //     node->AddChild(std::move(text_node));
    //   }
    //
    //   if (node->IsEmpty())
    //     return nullptr;
    //
    //   return node;
    // }

    // Parse until either '\n\n' (exit) or the next list element is found ('* '
    // or '1.') If '\n\n', then create a node and exit
    auto element = ParseInlineListContent();
    node->AddChild(std::move(element));
    // for (auto &child : children) {
    //   node->AddChild(std::move(child));
    // }

    char c = Peek();
    char c_next = Peek(1);

    // 2. Parser unordered list
    if (c == '*' || c == '-' || c == '+') {
      if (c_next == ' ' || c_next == '\t') {
        continue;
      }
    }

    // 3. Parse ordered list
    // TODO: This only checks a single digit, should check for 'n' digits
    if (std::isdigit(c) && c_next == '.') {
      continue;
    }

    break;
  }

  return node;
};

std::unique_ptr<Node> Parser::ParseCodeBlock() {
  auto node = std::make_unique<CodeBlockNode>();
  string str;

  // Remove the first three characters, the '```'
  Consume(3);

  // Parse text into a single text node until '```' is found, include everything
  // else
  while (!IsEOF()) {
    char c = Peek();
    if (c == '`' && Peek(1) == '`' && Peek(2) == '`') {
      Consume(3);
      break;
    }

    // Swap any '\n' with BR tags, so it will visually break
    if (c == '\n')
      str += "\n<br>\n";
    else
      str += c;

    Consume();
  }

  auto text_node = std::make_unique<TextNode>(str);
  node->AddChild(std::move(text_node));

  return node;
}

vector<std::unique_ptr<Node>> Parser::ParseInline() {
  vector<std::unique_ptr<Node>> nodes;
  string str;

  while (!IsEOF()) {
    char c = Peek();
    char c_next = Peek(1);
    // If this char and next char are both newlines: then we have an empty line,
    // we should stop.
    if (c == '\n' && Peek(1) == '\n')
      break;

    if (c == '!' && c_next == '[') {
      PushTextNode(nodes, str);
      auto node = ParseImage();
      if (!node->IsEmpty())
        nodes.push_back(std::move(node));
      continue;
    }

    if (c == '[') {
      PushTextNode(nodes, str);
      auto node = ParseLink();
      if (!node->IsEmpty())
        nodes.push_back(std::move(node));
      continue;
    }

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

vector<std::unique_ptr<Node>> Parser::ParseInlineHeading() {
  vector<std::unique_ptr<Node>> nodes;
  string str;

  while (!IsEOF()) {
    char c = Peek();
    char c_next = Peek(1);
    // We can stop as soon as we see a new line. Headings are single line blocks
    if (c == '\n')
      break;

    if (c == '!' && c_next == '[') {
      PushTextNode(nodes, str);
      auto node = ParseImage();
      if (!node->IsEmpty())
        nodes.push_back(std::move(node));
      continue;
    }

    if (c == '[') {
      PushTextNode(nodes, str);
      auto node = ParseLink();
      if (!node->IsEmpty())
        nodes.push_back(std::move(node));
      continue;
    }

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

std::unique_ptr<Node> Parser::ParseInlineListContent() {
  vector<std::unique_ptr<Node>> children;
  string str;

  while (!IsEOF()) {
    char c = Peek();
    char c_next = Peek(1);
    // If this char and next char are both newlines: then we have an empty line,
    // we should stop.
    if (c == '\n' && Peek(1) == '\n')
      break;

    // A single newline: We should consume whitespace and check if the next
    // character is a list item and the following item is a space
    if (c == '\n') {
      PushTextNode(children, str);
      ConsumeWhiteSpace();
      char new_c = Peek();
      char new_c_next = Peek(1);

      if ((new_c == '*' || new_c == '-' || new_c == '+') &&
          (new_c_next == ' ' || new_c_next == '\t'))
        break;

      if (std::isdigit(new_c) && new_c_next == '.')
        break;

      str += ' ';
      continue;
    }

    if (c == '!' && c_next == '[') {
      PushTextNode(children, str);
      auto node = ParseImage();
      if (!node->IsEmpty())
        children.push_back(std::move(node));
      continue;
    }

    if (c == '[') {
      PushTextNode(children, str);
      auto node = ParseLink();
      if (!node->IsEmpty())
        children.push_back(std::move(node));
      continue;
    }

    if (c == '*' && Peek(1) == '*' && Peek(2) == '*') {
      PushTextNode(children, str);
      auto node = ParseBoldItalic();
      if (!node->IsEmpty())
        children.push_back(std::move(node));
      continue;
    } else if (c == '*' && Peek(1) == '*') {
      PushTextNode(children, str);
      auto node = ParseBold();
      if (!node->IsEmpty())
        children.push_back(std::move(node));
      continue;
    } else if (c == '*') {
      PushTextNode(children, str);
      auto node = ParseItalic();
      if (!node->IsEmpty())
        children.push_back(std::move(node));
      continue;
    }

    if (c == '`') {
      PushTextNode(children, str);
      auto node = ParseCode();
      if (!node->IsEmpty())
        children.push_back(std::move(node));
      continue;
    }

    // If a newline, use a space instead
    str += (c == '\n' ? ' ' : c);
    Consume();
  }

  // Push the last node, if the string is not empty
  PushTextNode(children, str);

  // Create the list node with the children appended
  auto element = std::make_unique<ListElementNode>();
  for (auto &child : children) {
    element->AddChild(std::move(child));
  }

  return element;
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

std::unique_ptr<Node> Parser::ParseLink() {
  // Consume '['
  Consume();

  string content;
  while (!IsEOF()) {
    char c = Peek();
    if (c == ']')
      break;

    content += c;
    Consume();
  }

  // Consume ']('
  Consume(2);

  string link;
  while (!IsEOF()) {
    char c = Peek();
    if (c == ')') {
      Consume();
      break;
    }

    link += c;
    Consume();
  }

  return std::make_unique<LinkNode>(link, content);
}

std::unique_ptr<Node> Parser::ParseImage() {
  // Consume '!['
  Consume(2);

  string alt;
  while (!IsEOF()) {
    char c = Peek();
    if (c == ']')
      break;

    alt += c;
    Consume();
  }

  // Consume ']('
  Consume(2);

  string src;
  while (!IsEOF()) {
    char c = Peek();
    if (c == ')') {
      Consume();
      break;
    }

    src += c;
    Consume();
  }

  return std::make_unique<ImageNode>(src, alt);
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
