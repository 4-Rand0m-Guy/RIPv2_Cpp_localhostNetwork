#ifndef RIP_RIPROUTEENTRY_H
#define RIP_RIPROUTEENTRY_H

#include <ctime>
#include <string>

class RIPRouteEntry {
    public:
        /**
         * Construct RIPRouteEntry with data from the network.
         *
         * @param data (pointer to unsigned char array[20])
         */
        RIPRouteEntry(unsigned char* data);

        /**
         *
         * @param afi - Address Family Identifier (here set to zero or 0xFFFF for authentication)
         * @param address - ID of local router
         * @param nextHop - router ID of next Hop of route
         * @param metric - cost of route
         */
        RIPRouteEntry(unsigned address, unsigned nextHop, unsigned metric);

        /**
         * Authentification Entry
         *
         * @param afi - First entry begins with '0xFFFF' (65535)
         * @param authenticationType - 2 (only simple password is supported)
         * @param password - the password
         */
        RIPRouteEntry(short afi, short authenticationType, char*password[16]);

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

        short afi; // Address Family Identifier
        short tag; // Route Tag (unused except when specifying authentication type)
        int address; // Address of remote router (Router IDs are being used)
        int subnetMask; // unused
        int nextHop; // Address (ie routerID) of immediate router in a path
        int metric; // cost of a path

        short authenticationType; // only type 2 (plain password) is supported
        char* authentication[16]; // the password

        time_t time;


        /**
        * Invoke this method if creating an Authentification Entry.
        *
        * @param afi - First entry begins with '0xFFFF' (65535)
        * @param authenticationType - 2 (only simple password is supported)
        * @param password - the password
        */
        void init_authentication_entry(short afi, short authenticationType, char*password[16]);

        /**
         * Invoke this method if creating an RIP message from a local router
         * to send elsewhere (e.g. an update message).
         *
         * @param address (unsigned) - routerID of the local router
         * @param nextHop (unsigned) - Address (ie routerID) of immediate router in a path
         * @param metric (unsigned) - Cost of the path
         */
        void init_from_host(unsigned address, unsigned nextHop, unsigned metic);

        /**
         * Invoke this method if creating an RIP message with data that was
         * received from a remote router over the network.
         *
         * @param data (pointer to array of unsigned chars) - 20 bytes long char array making up a route entry
         */
        void init_from_net(unsigned char* data);

        /**
         * Initialize timer for entry.
         */
        void init_timeout();
};


#endif //RIP_RIPROUTEENTRY_H
