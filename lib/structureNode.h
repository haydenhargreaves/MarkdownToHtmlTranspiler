#ifndef STRUCTURENODE_H
#define STRUCTURENODE_H

#include "node.h"

class StructureNode : public Node {};

class ListNode : public StructureNode {};
class HeadingNode : public StructureNode {};
class DocumentNode : public StructureNode {};
class ParagraphNode : public StructureNode {};

#endif
