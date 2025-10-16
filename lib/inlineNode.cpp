#include "inlineNode.h"

#include <memory>
#include <stdexcept>
#include <string>

using std::string;

void InlineNode::AddChild(std::unique_ptr<Node> child) {
  throw std::runtime_error("Cannot add a child to an InlineNode.");
}

string TextNode::ToHtml() const { return this->content; }

string ItalicNode::ToHtml() const { return "<em>" + this->content + "</em>"; }

string BoldNode::ToHtml() const {
  return "<strong>" + this->content + "</strong>";
}

string BoldItalicNode::ToHtml() const {
  return "<strong><em>" + this->content + "</em></strong>";
}
