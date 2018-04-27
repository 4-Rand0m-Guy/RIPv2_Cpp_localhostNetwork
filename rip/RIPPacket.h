#ifndef RIP_RIPPACKET_H
#define RIP_RIPPACKET_H


#include <vector>
#include "RIPHeader.h"
#include "RIPRouteEntry.h"

class RIPPacket {

    public:
        /**
         * Invoke this constructor if creating an RIP message with data
         * that was received from a remote router over the network.
         *
         * @param data (pointer to unsigned char array) max length of 504.
         */
        RIPPacket(unsigned char * data, int length);

        /**
         * Invoke this constructor if creating an RIP message from a local router
         * to send else where (e.g. an update message).
         *
         * @param header - Rip Packet Header
         */
        RIPPacket(RIPHeader* _header);

        /**
         * @param _header
         * @param rtes
         */
        RIPPacket(RIPHeader* _header, std::vector<RIPRouteEntry> rtes);

        /**
         * Add a route to the routing table with char array.
         *
         * @param rte - the serialized routing entry
         */
        void addRoute(unsigned char * rte);

        void addRoute(RIPRouteEntry rte);

        /**
         * Deserialize data into RIPPacket.
         *
         * @param outBuffer - the serialized RIPPacket data received from network, max length 504 byte array.
         */
        void deserialize(std::istream &stream);


         /**
         * Function to help craft messages for RIP updates
         * @param packet a packet to be put into serialized vector
         * @return
         */
         void serialize(std::ostream &stream);

        /**
         * Returns a vector of all Route Entries in a packet.
         *
         * @return - vector<RIPRouteEntries>
         */
        std::vector<RIPRouteEntry> routes();

        /**
         * Returns RIPPacket as a human-readable string.
         *
         * @return string
         */
        std::string toString();

        RIPHeader getHeader();

    private:
        RIPHeader* header = new RIPHeader(new unsigned char);
        std::vector<RIPRouteEntry> routeEntries; // max length 25
        unsigned char * message;
        int cur_len;



};


#endif //RIP_RIPPACKET_H
