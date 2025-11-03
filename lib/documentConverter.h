#ifndef DOCUMENT_CONVERTER_H
#define DOCUMENT_CONVERTER_H

#include "commandLineParser.h"
#include "parser.h"
#include "watchdog.h"
#include <functional>

class DocumentConverter {
private:
  CLI cli;
  Parser parser;
  Watchdog watchdog;

public:
  DocumentConverter(int argc, char **argv)
      : cli(argc, argv), parser(cli.GetInputFile(), cli.GetOutputFile()),
        watchdog(cli.GetInputFile()) {}

  void Start() {
    if (this->cli.WatchDogEnabled()) {
      this->ConvertWatcher();
    } else {
      this->Convert();
    }
  }

private:
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
    if (cli.WatchDogEnabled()) {
      this->watchdog.Start(callback);
    }
  }
};

#endif
