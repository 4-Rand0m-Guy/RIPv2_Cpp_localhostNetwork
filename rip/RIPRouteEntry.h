#ifndef RIP_RIPROUTEENTRY_H
#define RIP_RIPROUTEENTRY_H

#include <ctime>
#include <string>

class RIPRouteEntry {
    public:
        /**
         * Invoke this constructor if creating an RIP message with data that was
         * received from a remote router over the network.
         *
         * @param data (pointer to unsigned char array) 20 bytes long char array making up a route entry
         */
        RIPRouteEntry(unsigned char* data);

        /**
         * Invoke this constructor if creating an RIP message from a local router
         * to send elsewhere (e.g. an update message).
         *
         * @param afi - Address Family Identifier (set to zero or 0xFFFF for authentication)
         * @param address - ID of destination router
         * @param nextHop - router ID of next Hop of route
         * @param metric - cost of route
         */
        RIPRouteEntry(unsigned _address, unsigned _nextHop, unsigned _metric);

        /**
         * Invoke this constructor if creating an Authentication Entry.
         *
         * @param FFFF - First entry begins with '0xFFFF' (65535)
         * @param authenticationType - 2 (only simple password is supported)
         * @param password - the password
         */
        RIPRouteEntry(short FFFF, short _authenticationType, char*password);

        /**
         * Deserialize data into RIPRouteEntry.
         *
         * @param outBuffer - the route entry data
         */
        void deserialize(unsigned char* outBuffer);

        /**
         * Serialize RIPRouteEntry into 20 byte Route entry.
         *
         * @param inBuffer
         */
        void serialize(unsigned char* inBuffer);

        /**
         * Returns RIPRouteEntry as a human-readable string.
         *
         * @return string
         */
        std::string toString();

    private:
        // unused fields are included for thoroughness and conformity to correct route entry table size
        unsigned short afi; // Address Family Identifier
        unsigned short tag; // Route Tag
        int address; // Address of remote router (Router IDs are being used)
        int subnetMask; // unused
        int nextHop; // Address (ie routerID) of immediate router in a path
        int metric; // cost of a path

        short authenticationType; // only type 2 (plain password) is supported
        char* authentication; // the password

        time_t time;

        /**
         * Initialize timer for entry.
         */
        void init_timer();
};


#endif //RIP_RIPROUTEENTRY_H
