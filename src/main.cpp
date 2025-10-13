#include "../lib/parser.h"
#include <stdexcept>

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Usage: <input_file> <?output_file>" << std::endl;
    return 0; // TODO: Should return 1?
  }

  try {
    if (argc >= 3) {
      Parser p(argv[1], argv[2]);
      p.Print();
    } else {
      Parser p(argv[1]);
      p.Print();
    }
  } catch (const std::runtime_error &e) {
    std::cout << "Caught an error: " << e.what() << std::endl;
  } catch (...) {
    std::cout << "Caught an error: UNKNOWN" << std::endl;
  }

  return 0;
}
