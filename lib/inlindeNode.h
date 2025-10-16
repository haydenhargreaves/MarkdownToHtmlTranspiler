#ifndef INLINENODE_H
#define INLINENODE_H

#include "node.h"
#include <string>

/**
 * @brief InlineNode class.
 *
 * Inherits from Node. Represents an inline object that is used
 * by the DOM tree to convert inline tags from a markup file
 * to an HTML file.
 *
 * @author Preston Shultz
 */
class InlineNode : public Node {
public:
    InlineNode(const std::string& content) : content(content) {}

    // Optional: allow access to the content
    std::string getContent() const { return content; }

protected:
    std::string content;
};

// These classes inherit the InlineNode constructor using `using`
class TextNode : public InlineNode {
public:
    using InlineNode::InlineNode;
};

class BoldNode : public InlineNode {
public:
    using InlineNode::InlineNode;
};

class ItalicNode : public InlineNode {
public:
    using InlineNode::InlineNode;
};

class BoldItalicNode : public InlineNode {
public:
    using InlineNode::InlineNode;
};

#endif // INLINENODE_H
