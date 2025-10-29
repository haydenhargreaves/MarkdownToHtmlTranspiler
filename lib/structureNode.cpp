#include "structureNode.h"
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <string>

using std::string;

string DocumentNode::ToHtml() const {
  std::stringstream ss;
  ss << "<!DOCTYPE html>\n\t<html lang=\"en\">\n\t\t<head>\n\t\t\t<meta "
        "charset=\"UTF-8\">\n\t\t\t<meta "
        "name=\"viewport\" content=\"width=device-width, "
        "initial-scale=1.0\">\n\t\t\t<title>Document</title>\n\t\t</"
        "head>\n\t<body>\n";

  for (const auto &child : this->GetChilren()) {
    ss << *child;
  }

  ss << "\n\t</body>\n</html>";
  return ss.str();
}

HeadingNode::HeadingNode(int size) {
  if (1 <= size && size <= 6) {
    this->size = size;
    return;
  }

  throw std::runtime_error("HeadingNode.size must be between 1 and 6.");
}

string HeadingNode::ToHtml() const {
  std::stringstream ss;
  ss << "<h" << size << ">";

  for (const auto &child : this->GetChilren()) {
    ss << *child;
  }

  ss << "</h" << size << ">\n";
  return ss.str();
}

string ParagraphNode::ToHtml() const {
  std::stringstream ss;
  ss << "<p>";

  for (const auto &child : this->GetChilren()) {
    ss << *child;
  }

  ss << "</p>\n";
  return ss.str();
}

string ListNode::ToHtml() const {
  std::stringstream ss;

  ss << (this->ordered ? "<ol>" : "<ul>") << "\n";

  for (const auto &child : this->GetChilren()) {
    ss << "<li>" << *child << "</li>" << "\n";
  }

  ss << (this->ordered ? "</ol>" : "</ul>") << "\n";
  return ss.str();
}

string CodeBlockNode::ToHtml() const {
  std::stringstream ss;

  ss << "<code>\n";

  for (const auto &child : this->GetChilren()) {
    ss << *child << "\n";
  }

  ss << "</code>\n";
  return ss.str();
}
