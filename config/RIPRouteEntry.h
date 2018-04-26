#ifndef RIP_RIPROUTEENTRY_H
#define RIP_RIPROUTEENTRY_H


class RIPRouteEntry {
    public:
        /**
         * Construct RIPRouteEntry with data from the network.
         *
         * @param data (pointer to unsigned char array)
         */
        RIPRouteEntry(unsigned char* data);

        RIPRouteEntry();
};


#endif //RIP_RIPROUTEENTRY_H
