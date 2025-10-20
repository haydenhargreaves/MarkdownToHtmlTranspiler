#ifndef NODE_H
#define NODE_H

#include <memory>
#include <string>
#include <vector>

/// NOTE: What the heck are unique pointers (unique_ptrs)
/// They are basically an abstraction over the typical (raw) pointer.
/// They handle ownership and deletion of the pointer for us, to prevent memory
/// leaks.
/// They are pretty easy to use, and only need to exist in places where new ones
/// are created. i.e., functions should not accept unique_ptrs, instead they
/// should accept normal pointers or references.
/// When calling a function that accepts a raw pointer, the unique_ptrs.get()
/// method is required. When calling a function that accepts a reference, the
/// reference operator (*) works perfectly fine. Hence, in this project we will
/// try to avoid using raw pointers, and only use references when needed.
/// Reference: https://www.youtube.com/watch?v=AmjoK55h68Y&t=166s

class Node {
protected:
  /**
   * @brief List of children nodes.
   *
   * Most nodes will not have children, but some may, therefore this class must
   * have it.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  std::vector<std::unique_ptr<Node>> children;

public:
  /**
   * @brief Inspect (view) the contents of the Node.
   *
   * This is a recursive approach to allow for indentation for easier viewing.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  virtual void Inspect(int indent = 0);

  /**
   * @brief Return the node as a string.
   *
   * In this ABC the content is just returned with no modifications. The child
   * nodes are expected to modify this behavior. i.e. wrapping in HTML tags.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  virtual std::string ToHtml() const = 0;

  virtual void AddChild(std::unique_ptr<Node> child) {
    // Move ownership from the existing owner, to this class
    this->children.push_back(std::move(child));
  }

  /**
   * @brief Return a read-only (const) list of children.
   *
   * Return our list of unique ptrs, they are const and therefore only have read
   * access.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  virtual const std::vector<std::unique_ptr<Node>> &GetChilren() const {
    return this->children;
  }

  /**
   * @brief Is the node empty.
   *
   * This is done differently for inline nodes and structure nodes.
   *
   * @author Hayden Hargreaves (hhargreaves2006@gmail.com)
   */
  virtual bool IsEmpty() const = 0;
};

#endif
