#ifndef RIP_RIP_H
#define RIP_RIP_H


#include <vector>
#include "../config/OutputInterface.h"

class Rip {
    public:
        Rip(unsigned routerID, std::vector<unsigned> input_ports, std::vector<OutputInterface> outputs, unsigned timer=30);

        /**
         * Function sets up the input sockets
         */
        void initializeInputPorts();

        /**
         * Function sets up the output ports
         */
        void initializeOutputPorts();

        /**
         * Function handles how the daemon runs. Listens to the setup ports for updates, handles requests and responses
         */
        void run();
    private:
        unsigned routerID;
        std::vector<unsigned> input_ports;
        std::vector<OutputInterface> outputs;
        unsigned timer;
};


#endif //RIP_RIP_H
