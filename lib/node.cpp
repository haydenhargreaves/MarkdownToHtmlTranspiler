#include "./node.h"

#include <iostream>
#include <memory>

void Node::Inspect(int indent) {

  if (this->children.size() > 0) {
    for (int i = 0; i <= indent; i++) {
      std::cout << "\t";
    }
    std::cout << "std::vector<Node> children: " << std::endl;
  }

  for (const auto &child : this->children) {
    for (int i = 0; i <= indent; i++) {
      std::cout << "\t";
    }
    child->Inspect(indent + 1);
  }
}
