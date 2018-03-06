//
// Created by Johny Appleseed on 3/6/18.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include "ConfigImporter.h"
#include "Utils.h"

ConfigImporter::ConfigImporter(int argc, char **argv) {
    std::ifstream file;
    const std::string filename = get_filename(argc, argv);
    open_config(filename, file);
    routerID = NULL;
    load_config(file);
};

std::string ConfigImporter::get_filename(int argc, char **argv) {
    if (argc < 2) {
        print_error(0);
    }
    if (argc > 2) {
        print_warning(0);
    }
    return argv[1];
}

void ConfigImporter::print_error(int errorCode) {
    std::string error("RIP:\033[1;31m error:\033[0m");
    std::string message = error_messages.at(errorCode);
    std::cout << error << ": " << message << std::endl;
    exit(0);
}

void ConfigImporter::print_warning(int errorCode) {
    std::string error("RIP:\033[1;33m warning:\033[0m");
    std::string message = warning_messages.at(errorCode);
    std::cout << error << ": " << message << std::endl;
}

void ConfigImporter::load_config(std::ifstream & cfgfile)
{
    char buffer[120];
    while (cfgfile.getline(buffer, 120))
    {
        char* tokBuffer;
        tokBuffer = strtok(buffer, ":");
        std::string key(tokBuffer);
        Utils::Strings::rtrim(key);
        if (tokBuffer != NULL) {
            if (key.at(0) == '#') {
                continue;
            }
            tokBuffer = strtok(NULL, "\n");
            if (tokBuffer != NULL) {
                std::string value(tokBuffer);
                Utils::Strings::ltrim(value);
                if (key == "router-id") {
                    std::cout << value << std::endl;
                    set_routerId(value);
                } else if (key == "input-ports") {
                    set_inputPorts(value);
                } else if (key == "outputs") {
                    std::cout << value << std::endl;
                } else if (key == "timer") {
                    std::cout << value << std::endl;
                } else {
                    std::cout << value << std::endl;
                    print_warning(1);
                }
            }
        }
    }
}

void ConfigImporter::open_config(const std::string &filename, std::ifstream& file) {
    if (!Utils::Files::isFileExist(filename)) {
        print_error(1);
    } else {
        file.open(filename.c_str());
    }
}

void ConfigImporter::set_inputPorts(std::string &inputs) {
    std::cout << inputs << std::endl;
}

void ConfigImporter::set_routerId(std::string &id) {
    set_generic_variable(id, routerID);
}

void ConfigImporter::set_generic_variable(std::string &rString, int &var) {
    if (var == NULL) {
        try {
            if (Utils::Strings::hasOnlyDigits(rString)) {
                var = std::stoi(rString);
            } else {
                throw std::invalid_argument("Not an integer.");
            }
        }
        catch (std::invalid_argument &ia) {
            print_error(3);
        }
    } else {
        print_warning(2);
    }
}

const std::unordered_map<int, std::string> ConfigImporter::error_messages({
    {0, "No input configuration file."},
    {1, "Configuration file could not be opened. Ensure that the file exists and is readable."},
    {2, "Some mandatory parameters are missing. Check the configuration file.\n"
        "\033[1mMandatory parameters:\033[0m\n"
        "\trouter-id\n\tinput-ports\n\toutputs\n"
        "\033[1mOptional parameters:\033[0m\n"
        "\ttimer"},
    {3, "router-id must be an integer."}
});

const std::unordered_map<int, std::string> ConfigImporter::warning_messages({
    {0, "Only one(1) configuration file taken as an argument. The rest of the arguments are ignored."},
    {1, "Invalid key in configuration file. Line ignored."},
    {2, "router-id is already defined. Ignoring."}
});

