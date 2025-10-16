#ifndef INLINENODE_H
#define INLINENODE_H

#include "node.h"
#include <vector>

/**
 * @desc ABC inline node for textual nodes.
 *
 * An inline node has content, but no children. An attempt to add children to
 * this node will result in a runtime exception.
 * Inline nodes define text itself, not textual tags, but the actual text inside
 * the tags.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class InlineNode : public Node {
public:
  InlineNode(std::string content) : content(content) {};

  /**
   * @brief Return node as HTML string.
   *
   * This method is virtual and required to be overridden.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  virtual std::string ToHtml() const = 0;

  /**
   * @brief Cannot add a child to this class.
   *
   * This method simply returns a runtime error, since there is no reason to add
   * a child to a textual node.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  void AddChild(std::unique_ptr<Node> child);

protected:
  std::string content;
};

/**
 * @desc A raw textual node.
 *
 * This node is simple, it simple returns it's content with no formatting.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class TextNode : public InlineNode {
public:
  TextNode(std::string content) : InlineNode(content) {};
  std::string ToHtml() const;
};

/**
 * @desc A italic textual node.
 *
 * This node returns it's content wrapped with <em></em> tags.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class ItalicNode : public InlineNode {
public:
  ItalicNode(std::string content) : InlineNode(content) {};
  std::string ToHtml() const;
};

/**
 * @desc A bold textual node.
 *
 * This node returns it's content wrapped with <strong></strong> tags.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class BoldNode : public InlineNode {
public:
  BoldNode(std::string content) : InlineNode(content) {};
  std::string ToHtml() const;
};

/**
 * @desc A bold and italic textual node.
 *
 * This node returns it's content wrapped with <strong></strong> AND <em>/<em>
 * tags.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class BoldItalicNode : public InlineNode {
public:
  BoldItalicNode(std::string content) : InlineNode(content) {};
  std::string ToHtml() const;
};

#endif
