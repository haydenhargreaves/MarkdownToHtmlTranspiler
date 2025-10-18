#include "../lib/inlineNode.h"
#include "../lib/parser.h"
#include "../lib/structureNode.h"
#include "../lib/watchDog.h"

#include <memory>
#include <stdexcept>

void test_nodes() {
  // For simplicity
  using std::make_unique;
  using std::unique_ptr;

  DocumentNode root;
  unique_ptr<TextNode> node = make_unique<TextNode>("text node");
  unique_ptr<BoldNode> bold = make_unique<BoldNode>("bold node");
  unique_ptr<ItalicNode> italic = make_unique<ItalicNode>("italic node");
  unique_ptr<BoldItalicNode> bolditalic =
      make_unique<BoldItalicNode>("bold italic node");

  unique_ptr<HeadingNode> heading = make_unique<HeadingNode>(2);
  heading->AddChild(std::move(node));
  heading->AddChild(std::move(bold));

  std::unique_ptr<ParagraphNode> para = std::make_unique<ParagraphNode>();
  para->AddChild(std::move(italic));
  para->AddChild(std::move(bolditalic));

  unique_ptr<ListNode> list = make_unique<ListNode>();

  root.AddChild(std::move(heading));
  root.AddChild(std::move(para));
  root.AddChild(std::move(list));

  std::cout << root.ToHtml() << std::endl;
}

/**
 *Preston: Test to see if watchdog works :)
 */
void test_watchdog() {
  WatchDog wd("test/input.md");
  wd.start();

  std::cout << "Initial check (should do nothing if file unchanged):\n";
  wd.checkFile();

  std::cout << "Now, modify or create the file 'example.txt' manually and "
               "press Enter:\n";
  std::cin.get(); // Wait for user to press Enter

  // Check again after manual change
  wd.checkFile();

  std::cout << "Done testing.\n";
}

void test_input(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Usage: <input_file> <?output_file>" << std::endl;
    return;
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
}

int main(int argc, char **argv) {
  Parser p("syntax.md");
  p.ParseDocument();
  p.WriteOutput();

  // Parser p2("README.md");
  // p2.ParseDocument();
}
