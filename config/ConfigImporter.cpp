#include <iostream>
#include <fstream>
#include <cstring>
#include "ConfigImporter.h"
#include "OutputInterface.h"
#include "../utils/Utils.h"
#include "ConsoleLogger.h"
#include "Configuration.h"

// Handles Import of router settings from config file
ConfigImporter::ConfigImporter(const std::string filename) {
    load_config(filename);
    bool isMissingConfigParams = routerID == NULL || input_ports.empty() || outputs.empty();
    if (isMissingConfigParams) { // timer is optional
        ConsoleLogger::error(2, "CONFIG");
    }
    if (timer == NULL) {
        timer = 30; // default is 30s
    }
};

Configuration ConfigImporter::get_configuration() {
    return Configuration({ routerID, input_ports, outputs, timer });
}

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
            bool isComment = key.at(0) == '#' || isspace(key.at(0));
            if (isComment) {
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
                } else if (key == "interfaces") {
                    set_outputs(value);
                } else if (key == "timer") {
                     set_timer(value);
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

// RouterID is the unique id of the router in the network
void ConfigImporter::set_routerId(std::string &id) {
    unsigned *p_routerID = &routerID;
    if (*p_routerID == NULL) {
        assign_variable_as_int(p_routerID, id);
    } else {
        ConsoleLogger::warning(2, "CONFIG");
    }
}

// Input ports are the ports that the RIP daemon will be listening on.
void ConfigImporter::set_input_ports(std::string &inputs) {
    if (input_ports.empty()) {
        std::vector<std::string> inputport_strings = Utils::Strings::split(inputs, ',');
        input_ports.resize(inputport_strings.size(), 0);
        int i = 0;
        for (unsigned int &input_port : input_ports) {
            unsigned *p = &input_port; // the address of the unsigned int pointed to by the iterator
            assign_variable_as_int(p, inputport_strings[i++]);
        }
    } else {
        ConsoleLogger::warning(2, "CONFIG");
    }
}

// Outputs contains information about the port number, id, and metric of adjacent routers.
void ConfigImporter::set_outputs(std::string &outputsline) {
    if (outputs.empty()) {
        for (auto output : Utils::Strings::split(outputsline, ',')) {
            std::vector<std::string> output_info = Utils::Strings::split(output, '-');
            if (output_info.size() > 3) {
                ConsoleLogger::error(4, "CONFIG");
            }
            OutputInterface oi{};
            // array of pointers to the unsigned ints in the oi struct
            unsigned* oi_array[3];
            oi_array[0] = &oi.port_number; oi_array[1] = &oi.metric; oi_array[2] = &oi.id;

            assign_variable_as_int(&oi.port_number, output_info[0]);
            assign_variable_as_int(&oi.metric, output_info[1]);
            assign_variable_as_int(&oi.id, output_info[2]);

            std::cout << "Port number: " << oi.port_number << ", ID is : " << oi.id << " and metric is : " << oi.metric
                      << std::endl;
            outputs.push_back(oi);
        }
    } else {
        ConsoleLogger::warning(2, "CONFIG");
    }
    for (auto out: outputs) {

    }
}

// RIP timer that certain timed updates are based on
void ConfigImporter::set_timer(std::string &_timer) {
    unsigned *p_timer = &timer;
    if (*p_timer == NULL) {
        assign_variable_as_int(p_timer, _timer);
    } else {
        ConsoleLogger::warning(2, "CONFIG");
    }
}

// Set variable, throw error if cannot convert to integer
void ConfigImporter::assign_variable_as_int(unsigned *variable, std::string &s) {
    try {
        if (Utils::Strings::hasOnlyDigits(s)) {
            auto i = static_cast<unsigned int>(std::stoi(s));
            if (i > 64000) {
                ConsoleLogger::error(5, "CONFIG");
            } else {
                *variable = i;
            }
        } else {
            throw std::invalid_argument("Not an integer.");
        }
    }
    catch (std::invalid_argument &ia) {
        ConsoleLogger::error(3, "CONFIG");
    }
}

