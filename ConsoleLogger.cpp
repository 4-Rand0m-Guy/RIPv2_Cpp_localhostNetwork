#include <unordered_map>
#include <iostream>
#include "ConsoleLogger.h"

// print warning
void ConsoleLogger::warning(int errorcode, std::string source) {
    std::string error("\033[1;33m warning:\033[0m");
    std::string message = warning_messages.at(errorcode);
    std::cout << source << ": " << error << ": " << message << std::endl;
}

// print error and exit program
void ConsoleLogger::error(int errorcode, std::string source) {
    std::string error("\033[1;31m error:\033[0m");
    std::string message = error_messages.at(errorcode);
    std::cout << source << ": " << error << ": " << message << std::endl;
    exit(0);
}

// errorcode maps to error message
const std::unordered_map<int, std::string> ConsoleLogger::error_messages({
      {0, "No input configuration file."},
      {1, "Unable to open onfiguration file. Ensure that the file exists and is readable."},
      {2, "Some mandatory parameters are missing. Check the configuration file.\n"
                  "\033[1mMandatory parameters:\033[0m\n"
                  "\trouter-id\n\tinput-ports\n\toutputs\n"
                  "\033[1mOptional parameters:\033[0m\n"
                  "\ttimer"},
      {3, "Configuration parameters must be an integer."}
});

// errorcode maps to warning message
const std::unordered_map<int, std::string> ConsoleLogger::warning_messages({
        {0, "Only one(1) configuration file taken as an argument. The rest of the arguments are ignored."},
        {1, "Invalid key in configuration file. Line ignored."},
        {2, "Multiple definitions of parameter. Ignoring."}
});