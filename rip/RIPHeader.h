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
         * Invoke this constructor if creating an RIP message with data that was
         * received from a remote router over the network.
         *
         * @param data (pointer to unsigned char array[4])
         */
        RIPHeader(unsigned char* data);

        /**
         * Invoke this constructor if creating an RIP message from a local router
         * to send elsewhere (e.g. an update message).
         *
         * @param _routerID (unsigned short) - ID of local router
         */
        RIPHeader(unsigned short _routerID);

        /**
         * Deserialize char array of size four into RIPHeader.
         *
         * @param outBuffer (pointer to char array[4]) - the header data
         */
        void deserialize(unsigned char* outBuffer);

        /**
         * Serialize RIPHeader into 4 byte char array.
         *
         * @param inBuffer - array to be filled
         */
        void serialize(unsigned char* inBuffer);

        /**
         * Get command type.
         *
         * @return (char) command
         */
        char getCommand();

        /**
         * Get version number.
         *
         * @return (char) version
         */
        char getVersion();

        /**
         * Get routerID
         *
         * @return (unsigned short) routerID
         */
        unsigned short getRouterID();

        /**
         * Returns RIPHeader as a human-readable string.
         *
         * @return string
         */
        std::string toString();

    private:

        char * command = new char(); // Two types of commands in RIP v1 and 2. 1 is type Request and 2 is Response.
        char * version = new char(); // RIP Version
        unsigned short * routerID = new unsigned short(); // ID of local Router
};


#endif //RIP_RIPHEADER_H
