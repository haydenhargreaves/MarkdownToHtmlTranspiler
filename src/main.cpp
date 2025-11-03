#include "../lib/documentConverter.h"

int main(int argc, char **argv) {
  DocumentConverter dc(argc, const_cast<char **>(argv));
  dc.Start();
};
