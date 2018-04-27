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
        RIPPacket(RIPHeader _header);

        /**
         * Add a route to the routing table.
         *
         * @param rte - the serialized routing entry
         */
        void addRoute(unsigned char * rte);

        /**
         * Deserialize data into RIPPacket.
         *
         * @param outBuffer - the serialized RIPPacket data received from network, max length 504 byte array.
         */
        void deserialize(unsigned char* outBuffer, int length);

        /**
         * Serialize RIPPacket into unsigned char array.
         *
         * @param inBuffer - array of unsigned chars
         */
        void serialize(unsigned char* inBuffer);

        /**
         * Returns RIPPacket as a human-readable string.
         *
         * @return string
         */
        std::string toString();

    private:
        RIPHeader* header = NULL;
        std::vector<RIPRouteEntry> routes; // max length 25
        unsigned char * message;
        int cur_len;
        int x;
};


#endif //RIP_RIPPACKET_H