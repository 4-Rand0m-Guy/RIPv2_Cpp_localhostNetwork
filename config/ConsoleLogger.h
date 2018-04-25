//
// Created by Johny Appleseed on 3/6/18.
//

#ifndef RIP_CONSOLELOGGER_H
#define RIP_CONSOLELOGGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>

class ConsoleLogger {
    public:
        static void error(int errorcode, std::string source);
        static void warning(int errorcode, std::string source);
    private:
        static const std::unordered_map<int, std::string> error_messages;
        static const std::unordered_map<int, std::string> warning_messages;
};


#endif //RIP_CONSOLELOGGER_H
