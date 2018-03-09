#ifndef RIP_CONFIGURATION_H
#define RIP_CONFIGURATION_H


#include <vector>
#include "OutputInterface.h"

struct Configuration {
    unsigned routerID;
    std::vector<unsigned> input_ports;
    std::vector<OutputInterface> outputs;
    unsigned timer;
};


#endif //RIP_CONFIGURATION_H
