#ifndef RIP_RIP_H
#define RIP_RIP_H


#include <vector>
#include "OutputInterface.h"

class Rip {
    public:
        Rip::Rip(unsigned routerID,
                 std::vector<unsigned> input_ports,
                 std::vector<OutputInterface> outputs,
                 unsigned timer=30);
    private:
        unsigned routerID;
        std::vector<unsigned> input_ports;
        std::vector<OutputInterface> outputs;
        unsigned timer;
};


#endif //RIP_RIP_H
