#ifndef RIP_CONFIGIMPORTER_H
#define RIP_CONFIGIMPORTER_H
#include <vector>
#include "OutputInterface.h"

class ConfigImporter {
    public:
        ConfigImporter(const std::string filename);
    private:
        unsigned routerID = NULL;
        std::vector<unsigned> input_ports;
        std::vector<OutputInterface> outputs;
//        std::vector<int*> outputs;
        unsigned timer = NULL;
        std::string get_filename(int argc, char **argv);
        void load_config(const std::string &filename);
        void open_config(const std::string &filename, std::ifstream &file);
        void set_input_ports(std::string &id);
        void set_outputs(std::string &outputs);
        void set_timer(std::string &timer);
        void assign_variable_as_int(unsigned *var, std::string &rString);
        void set_routerId(std::string &id);
};

#endif //RIP_CONFIGIMPORTER_H
