#ifndef DOCUMENT_CONVERTER_H
#define DOCUMENT_CONVERTER_H

#include "parser.h"
#include "watchdog.h"
#include <functional>

class DocumentConverter {
private:
  Parser parser;
  Watchdog watchdog;

public:
  DocumentConverter(std::string input, std::string output = "")
      : parser(input, output), watchdog(input) {};

  void Convert() {
    this->parser.ParseDocument();
    this->parser.WriteOutput();
  }

  void ConvertWatcher() {
    // This is a lambda, which can be passed into the start function of
    // watchdog.
    std::function<void()> callback = [this]() {
      this->parser.ParseDocument();
      this->parser.WriteOutput();
    };
    this->watchdog.Start(callback);
  }
};

#endif
