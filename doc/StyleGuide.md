# MarkdownToHtmlTranspiler – Style Guide

**Purpose:**  
This guide defines the coding, formatting, and contribution standards for all contributors to the **MarkdownToHtmlTranspiler** project. It ensures that all code remains consistent, maintainable, while supporting clarity.

---

## Project Overview

This project is a **C++ Markdown to HTML transpiler**, converting Markdown files into HTML using a structured parsing system.  
The repo includes:
- **Build tooling:** `Makefile`, `flake.nix`, and related Nix configurations  
- **Libraries & sources:** Under `src/` and `lib/`  
- **Docs & UML:** Project diagrams and notes describing architecture  

---

## ⚙️ 1. Repository Layout

| Path | Purpose |
|------|----------|
| `src/` | Main function
| `lib/` | Helper modules or shared logic reused across the project |
| `test/` | HTML output files |
| `Makefile` | Primary build automation script |
| `flake.nix` / `flake.lock` | Nix build and dependency management files |

### File Naming
- Source files: `camelCase.cpp`

---

## 💻 2. C++ Code Style

### General Formatting
- **Indentation:** 2 spaces (no tabs)
- **Line length:** 150 characters max
- **Braces:**
  ```cpp
  if (condition) {
    doSomething();
  } else {
    doSomethingElse();
  }
 
- **Spacing:**
  - Put a single space after commas and around binary operators.
  - No spaces after `(` or before `)`.
  - Example:
    ```cpp
    result = func(x, y + 1);
    ```
### Naming Conventions

- **Classes & Structs:** `PascalCase`  
  Example: `Parser`, `DomTree`
- **Functions & Methods:** `PascalCase`  
  Example: `Inspect()`, `ParserDocument()`
- **Variables & Parameters:** `snake_case`  
  Example: `input_file`, `output_path`
- **Constants:** `ALL_CAPS_WITH_UNDERSCORES`  
  Example: `MAX_BUFFER_SIZE`
- **Namespaces:** `lowercase`
---
### Error Handling

Use **exceptions** for runtime errors (`std::runtime_error`, `std::invalid_argument`, etc.).

Always catch by reference.

**Example:**

```cpp
try {
  Parser p(argv[1]);
  p.Inspect();
} catch (const std::runtime_error &e) {
  std::cerr << "Error: " << e.what() << std::endl;
}
 ```
 ##  3. Commenting & Documentation Style
 ###  General Rules

- Use `//` for short inline comments.
- Use `/** ... */` for function, class, or member documentation.
- Start each docblock with `@brief` for a concise description.
- Include `@param`, `@return`, and `@throws` (if applicable).
- Always describe the intent, not just what the code does.
- Add `@author` for major contributors.
- ### 🔹 Example: Class Documentation

Below is a properly documented example class showing how to format comments consistently throughout your codebase:
```cpp
/**
* @file commandLineParser.h
* @brief Parses command line
* @author Preston Shultz
* Sources:
*
*/

#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H
  
#include  <string>
#include  <vector>
  
/**

* @brief CommandLine Helper class
*
* Parse input arguments, sets input and output files
* Lets user use commands with program
*
* @author Preston Shultz (shultzp1@my.erau.edu)
*/
class  CLI {

public:
/**
* @brief Takes in argc and argv and converts it to vector
*
* CLI constructor
*
* @author Preston Shultz (shultzp1@my.erau.edu)
*/
CLI(int  argc, char  **argv);


/**
* @brief Runs Commands
*
* @author Preston Shultz (shultzp1@my.erau.edu)
*/
void  RunCommands();


/**
* @brief Prints a list of commands that can be used
*
* @author Preston Shultz (shultzp1@my.erau.edu)
*/
void  PrintHelp() const;


/**
* @brief Returns input file
*
* @author Preston Shultz (shultzp1@my.erau.edu)
*/
std::string  GetInputFile() const { return input_file; }


/**
* @brief Returns if watchdog is enabled
*
* @author Preston Shultz (shultzp1@my.erau.edu)
*/
bool  WatchDogEnabled() {return watchdog_enabled;}


/**
* @brief Returns output file
*
* @author Preston Shultz (shultzp1@my.erau.edu)
*/
std::string  GetOutputFile() const { return output_file; }


private:
/**
* @brief Ensures the provided filename has a .markup extension
*
* @return true if file ends with .markup
* @return false otherwise
* @author Preston Shultz (shultzp1@my.erau.edu)
*/
static  bool  IsMarkupFile(const  std::string  &filename);


/**
* @brief Ensures the provided filename has a .html extension
*
* @return true if file ends with .html
* @return false otherwise
* @author Preston Shultz (shultzp1@my.erau.edu)
*/
static  bool  IsHtmlFile(const  std::string  &filename);
  

/**
* @brief Parses the commands
*
* @author Preston Shultz (shultzp1@my.erau.edu)
*/
void  ParseArgs(int  argc, char**  argv);
  

std::string program_name;
std::string input_file;
std::string output_file =  "";
bool watchdog_enabled =  false;
};

#endif
