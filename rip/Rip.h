#ifndef RIP_RIP_H
#define RIP_RIP_H


#include <vector>
#include "../config/OutputInterface.h"

class Rip {
    public:
        Rip(unsigned routerID, std::vector<unsigned> input_ports, std::vector<OutputInterface> outputs, unsigned timer=30);

        /**
         * Function handles how the daemon runs. Listens to the setup ports for updates, handles requests and responses
         */
        void run();

        /**
         * Function to get the input ports
         * @return Vector: Vector containing the input ports registered with the daemon.
         */
        const std::vector<unsigned int> &getInput_ports() const;

private:
        unsigned routerID;
        std::vector<unsigned> input_ports;
        std::vector<OutputInterface> outputs;
        unsigned timer;

    /**
    * Function sets up the input sockets
    */
    void initializeInputPorts();


};


#endif //RIP_RIP_H
