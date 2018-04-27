#ifndef RIP_RIP_H
#define RIP_RIP_H


#include <vector>
#include "../config/OutputInterface.h"
#include "RIPRouteEntry.h"
#include "RIPHeader.h"

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

        /**
        * Checks each routing table entry included in a message that was received
        * from the network. If new routes are discovered it will attempt to
        * add them to its own routing table. If a more optimal alternative
        * path is discovered or an old path has been marked for expiration
        * then appropriate changes are made accordingly.
        *
        * @param message (array of unsigned chars) - data received from network
        */
        void processResponse(unsigned char * message);

private:
        unsigned routerID;
        std::vector<unsigned> input_ports;
        std::vector<OutputInterface> outputs;
        unsigned timer;
        std::vector<RIPRouteEntry> forwardingTable;

    /**
    * Function sets up the input sockets
    */
    std::vector<unsigned int> initializeInputPorts();

    /**
     * Invoke to crafts an RIP update message. Parameter isTriggered
     * has default value of false which implies a regular (timed)
     * update message. If isTriggered is True then a Triggered Update
     * Message will be crafted.
     *
     * Note: Split Horizon with poisoned reverse is implemented here.
     *
     * @param isTriggered
     */
    void craftUpdateMessage(bool isTriggered=false);
    /**
    * Function sends update to neighboring routers once time limit is reached.
    * @param fdValue Value of one of the socket() call return values, will be used to assign sending port
    */
    void sendUpdate(int fdValue);

    /**
     * Function generates a random time value in the range 0.8*timer and 1.2*timer
     * @return Double: Value of the random time in (s)
     */
    double randomTimeGenerator();

    /**
     * Function sets up the routing table ot it's initial state using the neighbors found in the config file and metrics
     */
    void initializeTable();



    /**
     * Function receives message at the provided port and processes it
     * @param fd: File descriptor for the socket receiving data
     */
    unsigned char * receive(unsigned int fd);

    /**
     * Function creates a RIP packet header to prepare sending a packet
     * @param routerID: The unique router ID that the
     * @return
     */
    RIPHeader createHeader();

    /**
     * Function checks if the next hop to a destination is the router about to be messaged. This for the
     * split-horizon with poison-reverse implementation
     * @param entry: A route entry from the table
     * @param output: Output interface to determine which neighbor is about to be messaged
     * @return bool: True if the next hop is the router to be messaged
     */
    bool nextHopIsRouter(RIPRouteEntry entry, OutputInterface output) {
        bool value =false;
        if (entry.getNextHop() == output.id) {
            value = true;
        }
        return value;
    }
};


#endif //RIP_RIP_H
