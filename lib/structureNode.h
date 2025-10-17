#ifndef STRUCTURENODE_H
#define STRUCTURENODE_H

#include "node.h"
#include <iostream>
#include <string>

/**
 * @desc ABC inline node for structure nodes.
 *
 * A structure node has no content, but instead it has children which contain
 * content. These children can be other structure nodes, or they can be inline
 * nodes. Inline nodes allow this node to display textual content.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class StructureNode : public Node {
public:
  /**
   * @brief Return node as HTML string.
   *
   * This method is virtual and required to be overridden.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  virtual std::string ToHtml() const = 0;

  /**
   * @brief Is the node empty.
   *
   * This is the same as checking if the node has no children.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  bool IsEmpty() const { return this->children.size() == 0; };
};

/**
 * @desc A main HTML document node.
 *
 * This node should be used as the root of the DOM. Calling `ToHtml` will yield
 * an HTML document boiler plate with it's children rendered as the body.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class DocumentNode : public StructureNode {
public:
  std::string ToHtml() const;
};

/**
 * @desc A heading node.
 *
 * A heading contains children that are expected to be inline nodes. These
 * children represent the textual content of the node. This heading node simply
 * stores the children and the size of the heading. A size of 1 means <h1>, etc.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class HeadingNode : public StructureNode {
protected:
  /**
   * @brief Size of the heading.
   *
   * The constructor will enforce size requirements.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  int size;

public:
  HeadingNode(int size);
  std::string ToHtml() const;
};

/**
 * @desc A paragraph node.
 *
 * A paragraph contains children that are expected to be inline nodes. These
 * children represent the textual content of the node. This heading node simply
 * stores the children.
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class ParagraphNode : public StructureNode {
public:
  std::string ToHtml() const;
};

/**
 * @desc A list container node.
 *
 * This node is used to wrap list elements. Not to be confused with the list
 * elements themselves. This is the parent (<ul> or <ol>), and must be created
 * with a boolean which defines whether the list should be ordered or not.
 * i.e., should have numbers (use the <ol> tags).
 *
 * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
 */
class ListNode : public StructureNode {
protected:
  /**
   * @brief Should the list be ordered (numeric).
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  bool ordered;

public:
  ListNode(bool ordered = false) : ordered(ordered) {};
  std::string ToHtml() const;
};

#endif
