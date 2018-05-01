#ifndef RIP_CONFIGIMPORTER_H
#define RIP_CONFIGIMPORTER_H
#include <vector>
#include "OutputInterface.h"
#include "Configuration.h"

class ConfigImporter {
    public:
        /**
         * ConfigImporter Constructor
         *
         * @param filename
         */
        ConfigImporter(const std::string filename);

        /**
         * Gets the struct Configuration containg the configuration settings.
         *
         * @return struct Configuration
         */
        struct Configuration get_configuration();
    private:
        unsigned routerID = NULL;
        std::vector<unsigned> input_ports;
        std::vector<OutputInterface> outputs;
        unsigned timer = NULL;
        std::string get_filename(int argc, char **argv);

        /**
         * Loads variables from configuration file into program settings.
         *
         * @param filename - Reference to String
         */
        void load_config(const std::string &filename);

        /**
         * Opens a configuration file for reading.
         *
         * @param filename - const reference to String
         * @param file - reference to ifstream
         */
        void open_config(const std::string &filename, std::ifstream &file);

        /**
         * Loads the input ports into an instance variable vector<string>.
         *
         * @param inputs - reference to String
         */
        void load_input_ports(std::string &id);

        /**
         * Loads the outputs into an instance variable vector<string>
         *
         * @param outputs - reference to String
         */
        void load_outputs(std::string &outputs);

        /**
         * Loads the timer (optional) into an instance variable unsigned.
         *
         * @param timer - reference to String
         */
        void load_timer(std::string &timer);

        /**
         * Ensure that configuration variables are convertible into integers.
         *
         * @param var  - pointer to unsigned, variable to be returned
         * @param rString - reference to string from the configuration file
         */
        void assign_variable_as_int(unsigned *var, std::string &rString);
        void set_routerId(std::string &id);
};

#endif //RIP_CONFIGIMPORTER_H
