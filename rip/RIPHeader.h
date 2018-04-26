#ifndef RIP_RIPHEADER_H
#define RIP_RIPHEADER_H

#include "istream"

/**
 * Every message contains an RIP Header which consists of a command, version number,
 * and router ID of the originating router.
 */
class RIPHeader {

    public:

        /**
         * Construct RIPHeader with data from the network.
         *
         * @param data (pointer to unsigned char array[4])
         */
        RIPHeader(unsigned char* data);

        /**
         * Construct with data from the host.
         *
         * @param routerID (short) - ID of local router
         */
        RIPHeader(short routerID);

        /**
         * Deserialize char array of size four into RIPHeader.
         *
         * @param outBuffer (pointer to char array[4]) - the header data
         */
        void deserialize(unsigned char* outBuffer);

        /**
         * Serialize RIPHeader into four byte char array.
         *
         * @param inBuffer - array to be filled
         */
        void serialize(unsigned char* inBuffer);

        /**
         * Returns RIPHeader as a human-readable string.
         *
         * @return string
         */
        std::string toString();

    private:

        char command; // Two types of commands in RIP v1 and 2. 1 is type Request and 2 is Response.
        char version; // RIP Version
        short routerID; // ID of local Router

        /**
         * Invoke this method if creating an RIP message from a local router
         * to send elsewhere (e.g. an update message).
         *
         * @param routerID (short integer) - routerID of the local router
         */
        void init_from_host(short routerID);

        /**
         * Invoke this method if creating an RIP message with data that was
         * received from a remote router over the network.
         *
         * @param data (pointer to array of unsigned chars) - first four bytes of data is the RIP Header.
         */
        void init_from_net(unsigned char* data);
};


#endif //RIP_RIPHEADER_H
