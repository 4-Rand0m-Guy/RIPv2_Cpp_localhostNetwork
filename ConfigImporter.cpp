#include <iostream>
#include <fstream>
#include <cstring>
#include "ConfigImporter.h"
#include "OutputInterface.h"
#include "Utils.h"
#include "ConsoleLogger.h"

// Handles Import of router settings from config file
ConfigImporter::ConfigImporter(const std::string filename) {
    routerID = -1;
//    input_ports = -1;
//    outputs = -1;
    timer = -1;
    load_config(filename);
};

// reads config settings into program variables
void ConfigImporter::load_config(const std::string &filename) {
    std::ifstream configfile;
    open_config(filename, configfile);
    char buffer[120];
    while (configfile.getline(buffer, 120))
    {
        char* tokBuffer;
        tokBuffer = strtok(buffer, ":");
        if (tokBuffer != NULL) {
            std::string key(tokBuffer);
            Utils::Strings::rtrim(key);
            if (key.at(0) == '#' || isspace(key.at(0))) {
                continue;
            }

            tokBuffer = strtok(NULL, "\n");
            if (tokBuffer != NULL) {
                std::string value(tokBuffer);
                Utils::Strings::ltrim(value);
                if (key == "router-id") {
                    set_routerId(value);
                } else if (key == "input-ports") {
                    set_input_ports(value);
                } else if (key == "outputs") {
                    set_outputs(value);
                } else if (key == "timer") {
                    // set timer
                } else {
                    ConsoleLogger::warning(1, "CONFIG");
                }
            }
        }
    }
}

// opens config file for reading
void ConfigImporter::open_config(const std::string &filename, std::ifstream &file) {
    if (!Utils::Files::isFileCanOpen(filename)) {
        ConsoleLogger::error(1, "CONFIG");
    } else {
        file.open(filename.c_str());
    }
}

void ConfigImporter::set_outputs(std::string &outputsline) {
//    for (auto output : Utils::Strings::split(outputsline, ',')) {
//        for (auto var : Utils::Strings::split(output, '-')) {
//            OutputInterface outputInterface = {};
//        }
}

void ConfigImporter::set_input_ports(std::string &inputs) {
    std::vector<std::string> inputports = Utils::Strings::split(inputs, ',');
    for (auto input : inputports) {
        inputports.push_back(input);
    }
}

void ConfigImporter::set_routerId(std::string &id) {
    int *p_routerID = &routerID;
    assign_variable_as_int(p_routerID, id);
    std::cout << routerID << std::endl;
}

// Set variable, throw error if cannot convert to integer, print warning if already set
void ConfigImporter::assign_variable_as_int(int *variable, std::string &s) {
    if (*variable < 0) {
        try {
            if (Utils::Strings::hasOnlyDigits(s)) {
                *variable = std::stoi(s);
            } else {
                throw std::invalid_argument("Not an integer.");
            }
        }
        catch (std::invalid_argument &ia) {
            ConsoleLogger::error(3, "CONFIG");
        }
    } else {
//        std::cout <<
        ConsoleLogger::warning(2, "CONFIG");
    }
}

