#include "commandLineParser.h"
#include <iostream>

CLI::CLI(int argc, char** argv){
    if (argc > 0){
        programName = argv[0];
    }

    for (int i = 1; i < argc; i++){
        args.push_back(argv[i]);
    }
}

void CLI::RunCommands(){
    for (int i = 0; i < args.size(); i++){
        
    }    
}

void CLI::PrintHelp() const{
    std::cout << "Usage:\n"
              << " " << programName << " <input_file>\n"
              << "Flags:\n"
              << " --o, --output <outputFile>, optional output filename\n"
              << " --w, --watch, enables watchdog\n"
              << " --s, --stop, stops watchdog\n";
}

