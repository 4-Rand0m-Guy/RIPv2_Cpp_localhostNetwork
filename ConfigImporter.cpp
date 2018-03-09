#include <iostream>
#include <fstream>
#include <cstring>
#include "ConfigImporter.h"
#include "OutputInterface.h"
#include "Utils.h"
#include "ConsoleLogger.h"

// Handles Import of router settings from config file
ConfigImporter::ConfigImporter(const std::string filename) {
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

// Outputs contains information about the port number, id, and metric of adjacent routers.
void ConfigImporter::set_outputs(std::string &outputsline) {
    for (auto output : Utils::Strings::split(outputsline, ',')) {
        std::vector<std::string> output_info = Utils::Strings::split(output, '-');
        if (output_info.size() > 3) {
            ConsoleLogger::error(4, "CONFIG");
        }
        OutputInterface oi;
        // array of pointers to the unsigned ints in the oi struct
        unsigned *oi_array[3] = {&oi.port_number, &oi.id, &oi.metric};
        unsigned *p = oi_array[0];
        for (auto var : output_info) {
            assign_variable_as_int(p++, var);
        }
        outputs.push_back(oi);
    }
}

// Input ports are the ports that the RIP daemon will be listening on.
void ConfigImporter::set_input_ports(std::string &inputs) {
    if (input_ports.empty()) {
        std::vector<std::string> inputport_strings = Utils::Strings::split(inputs, ',');
        input_ports.resize(inputport_strings.size(), 0);
        int i = 0;
        for (auto it = input_ports.begin(); it != input_ports.end(); ++it) {
            unsigned *p = &(*it); // the address of the integer pointed to by the iterator
            assign_variable_as_int(p, inputport_strings[i++]);
        }
    } else {
        ConsoleLogger::warning(2, "CONFIG");
    }
}

// RouterID is the unique id of the router in the network
void ConfigImporter::set_routerId(std::string &id) {
    unsigned *p_routerID = &routerID;
    if (*p_routerID == NULL) {
        assign_variable_as_int(p_routerID, id);
    } else {
        ConsoleLogger::warning(2, "CONFIG");
    }
}

// Set variable, throw error if cannot convert to integer, print warning if already set
void ConfigImporter::assign_variable_as_int(unsigned *variable, std::string &s) {
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
}

