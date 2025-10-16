#include "../lib/inlineNode.h"
#include "../lib/parser.h"
#include "../lib/structureNode.h"

#include <memory>
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

  DocumentNode root;
  std::unique_ptr<TextNode> node = std::make_unique<TextNode>("text node");
  std::unique_ptr<BoldNode> bold = std::make_unique<BoldNode>("bold node");
  std::unique_ptr<ItalicNode> italic =
      std::make_unique<ItalicNode>("italic node");
  std::unique_ptr<BoldItalicNode> bolditalic =
      std::make_unique<BoldItalicNode>("bold italic node");

  std::unique_ptr<HeadingNode> heading = std::make_unique<HeadingNode>(2);
  heading->AddChild(std::move(node));
  heading->AddChild(std::move(bold));

  std::unique_ptr<ParagraphNode> para = std::make_unique<ParagraphNode>();
  para->AddChild(std::move(italic));
  para->AddChild(std::move(bolditalic));

  std::unique_ptr<ListNode> list = std::make_unique<ListNode>();

  root.AddChild(std::move(heading));
  root.AddChild(std::move(para));
  root.AddChild(std::move(list));

  std::cout << root.ToHtml() << std::endl;

  return 0;
}

/**
 *Preston: Test to see if watchdog works :)
 */

// #include "../lib/watchDog.h"
// #include <iostream>
//
// int main() {
//   WatchDog wd("test/input.md");
//   wd.start();
//
//   std::cout << "Initial check (should do nothing if file unchanged):\n";
//   wd.checkFile();
//
//   std::cout << "Now, modify or create the file 'example.txt' manually and "
//                "press Enter:\n";
//   std::cin.get(); // Wait for user to press Enter
//
//   // Check again after manual change
//   wd.checkFile();
//
//   std::cout << "Done testing.\n";
//   return 0;
// }
