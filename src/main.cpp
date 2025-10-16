/*#include "../lib/parser.h"
#include <stdexcept>

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Usage: <input_file> <?output_file>" << std::endl;
    return 0; // TODO: Should return 1?
  }

  try {
    if (argc >= 3) {
      Parser p(argv[1], argv[2]);
      p.Inspect();
    } else {
      Parser p(argv[1]);
      p.Inspect();
    }
  } catch (const std::runtime_error &e) {
    std::cout << "Caught an error: " << e.what() << std::endl;
  } catch (...) {
    std::cout << "Caught an error: UNKNOWN" << std::endl;
  }

  std::cout << std::endl;

  return 0;
}
*/

/**
 *Preston: Test to see if watchdog works :)
 */

#include "watchDog.h"
#include <iostream>

int main() {
    WatchDog wd("test/input.md");
    wd.start();

    std::cout << "Initial check (should do nothing if file unchanged):\n";
    wd.checkFile();

    std::cout << "Now, modify or create the file 'example.txt' manually and press Enter:\n";
    std::cin.get(); //Wait for user to press Enter

    //Check again after manual change
    wd.checkFile();

    std::cout << "Done testing.\n";
    return 0;
}