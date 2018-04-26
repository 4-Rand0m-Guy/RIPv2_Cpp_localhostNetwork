#ifndef RIP_RIP_H
#define RIP_RIP_H


#include <vector>
#include "../config/OutputInterface.h"
#include "RIPRouteEntry.h"

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
        std::vector<RIPRouteEntry> forwardingTable;

    /**
    * Function sets up the input sockets
    */
    std::vector<int> initializeInputPorts();

    /**
    * Function sends update to neighboring routers once time limit is reached.
    * @param fdValue Value of one of the socket() call return values, will be used to assign sending port
    */
    void sendUpdate(int fdValue);


};


#endif //RIP_RIP_H
