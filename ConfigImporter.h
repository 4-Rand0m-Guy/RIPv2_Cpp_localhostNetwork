//
// Created by plr37 on 3/6/18.
//

#ifndef RIP_CONFIGIMPORTER_H
#define RIP_CONFIGIMPORTER_H
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "OutputInterface.h"

class ConfigImporter {
    static const std::unordered_map<int, std::string> error_messages;
    static const std::unordered_map<int, std::string> warning_messages;
    public:
        ConfigImporter(int argc, char **argv);
    private:
        std::string filename;
        int routerID;
        std::vector<int> input_ports;
        std::vector<OutputInterface> outputs;
        int timer;
        void print_error(int errorCode);
        void print_warning(int errorCode);
        std::string get_filename(int argc, char **argv);
        void load_config(std::ifstream &file);
        void open_config(const std::string &filename, std::ifstream &file);
        void set_inputPorts(std::string &id);
        void set_generic_variable(std::string &rString, int &var);
        void set_routerId(std::string &id);
};

#endif //RIP_CONFIGIMPORTER_H
