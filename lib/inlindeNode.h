#ifndef INLINENODE_H
#define INLINENODE_H
#include "node.h"

class InlineNode : public Node{
    public:
        InlineNode(std::string content) : content(content);
    protected:
        std::string content;
};

class TextNode : public InlineNode{};
class BoldNode : public InlineNode{};
class Italic : public InlineNode{};
class BoldItalic : public InlineNode{};

#endif 
