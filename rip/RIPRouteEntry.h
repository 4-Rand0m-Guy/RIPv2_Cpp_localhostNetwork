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
        RIPRouteEntry(short FFFF, short _authenticationType, unsigned char* password);

        /**
         * Deserialize data into RIPRouteEntry.
         *
         * @param outBuffer - the serialized route entry data, 20 byte unsigned char array
         */
        void deserialize(unsigned char* outBuffer);

        /**
         * Serialize RIPRouteEntry into 20 byte Route entry.
         *
         * @param inBuffer - 20 byte array[20] of unsigned chars
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
        unsigned char authentication[16]; // the password

        time_t time;

        /**
         * Initialize timer for entry.
         */
        void init_timer();

        /**
         * Characters-to-Unsigned Integer. Reads four chars (4 bytes) into an
         * unsigned integer. Ensure that index is at least 4 indices
         * from the end of the array.
         *
         * @param chars - array of chars
         * @param index - index in the array to begin
         * @return an unsigned integer
         */
        unsigned ch2uint(unsigned char* chars, int index);

        /**
         * Unsigned Integer to Character. Pass an unsigned integer,
         * a buffer (with at least 4 indices) and an index at least
         * 4 indices from the end. The four bytes of the uint will
         * be written to the buffer.
         *
         * @param ui - unsigned integer
         * @param buf - buffer, pointer to an array of unsigned chars
         * @param index - index in the buffer to convert from (needs 4 bytes)
         */
        void uint2ch(unsigned ui, unsigned char* buf, int index);
};


#endif //RIP_RIPROUTEENTRY_H
