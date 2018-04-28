#ifndef RIP_RIP_H
#define RIP_RIP_H
#define DGRAM_SIZE 504
#define HOST "127.0.0.1"
#define RIP_VERSION '2'
#define HEADER_SIZE 4
#define RTE_SIZE 20

#include <map>
#include <vector>
#include "../config/OutputInterface.h"
#include "rip_client_server.h"

typedef char byte;

typedef struct Rip_header {
    /**
     * Header Struct for rip packets
     */
    byte command; 		   /* 1-REQUEST, 2-RESPONSE */
    byte version;          /* 1-RIPv1, 2-RIPv2 */
    short int routerID;    /* routerID */
} Header;

typedef struct Rip_entry {
    /**
     * RIP Entry struct for entries in packets.
     */
    short afi;
    short tag;
    int ipaddress;
    int subnetmask;
    int nextHop;
    int metric;
} Entry;

typedef struct Rip_packet {
    /**
     * RIP Packet
     */
    Rip_header header;
    std::vector<Rip_entry> entries;
} Packet;

typedef struct Route_table_entry {
    /**
     * Struct representing one single route table entry (RTE)
     * in the routing table.
     */
    int destination;
    int metric;
    int nexthop;
    byte routechanged;		/*route change flag */
    void *timeout_tmr;
    void *garbage_tmr;
} RTE;

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
        void processResponse(Rip_packet packet);

private:
        unsigned routerID;
        std::vector<unsigned> input_ports;
        std::vector<OutputInterface> outputs;
        unsigned timer;
//////////////////////////////////////////////
    std::vector<Route_table_entry> routingTable;
    std::vector<rip_client_server::rip_server*> servers;
    std::vector<rip_client_server::rip_client*> clients;
    std::vector<int> output_ports;


    /**
     * Initialize. Loads configuration settings, initalizes instance variables,
     * client and server sockets, and starts up the RIP daemon (todo)
     */
    void init();

    /**
     * Initialize sockets for the receiving of UDP packets. Available servers
     * can be accessed in the private instance variable servers.
     */
    void initializeServers();

    /**
     * Initialize sockets for the sending of UDP packets. Available clients
     * can be accessed in the private instance variable clients.
     */
    void initializeClients();

    /**
     * Use this method on messages received from a socket on the network.
     * This converts a message of byte data to an Rip_packet.
     *
     * @param message - raw data straight off the socket
     * @param size - number of bytes received from socket.
     * @return struct Rip_packet
     */
    Rip_packet deserialize_rip_message(char *message, int size);

    /**
     * Adds a header to a yet so far empty message.
     *
     * @param message - empty message of sufficient size to eventually hold the header and
     * all the RTEs.
     * @return the new message with a header included
     */
    char* add_header(char *message);

    /**
     * Adds another route entry to a message.
     *
     * @param message - message to add RTEs to
     * @param entry - Route Table Entry to glean information from for the rip entry
     * @return the new message with another RTE included at bottom
     */
    char* add_RTE(char *message, struct Route_table_entry entry);

    /**
     * Prints rip entry to console.
     *
     * @param entry
     */
    void print_entry(struct Rip_entry entry);

    /**
     * Prints rip header to console.
     *
     * @param header
     */
    void print_header(struct Rip_header header);

    /**
     * Generates an RIP response message. Parameter isTriggered
     * has default value of false which implies a regular (timed)
     * update message. If isTriggered is True then a Triggered Update
     * Message will be generated.
     *
     * todo Split Horizon with poisoned reverse is implemented here.
     *
     * @param isTriggered
     */
    char* generate_response(char* msg, int size, bool isTriggered=false);
    /**
    * Function sets up the input sockets
    */
    std::vector<unsigned int> initializeInputPorts();


    /**
    * Function sends update to neighboring routers once time limit is reached.
    * @param fdValue Value of one of the socket() call return values, will be used to assign sending port
    */
    void send_message(int fdValue, char* message, size_t size);


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
     * Function checks if the next hop to a destination is the router about to be messaged. This for the
     * split-horizon with poison-reverse implementation
     * @param entry: A route entry from the table
     * @param output: Output interface to determine which neighbor is about to be messaged
     * @return bool: True if the next hop is the router to be messaged
     */
    bool nextHopIsRouter(Route_table_entry entry, OutputInterface output);

    /**
     * Get the cost from host to neighbor.
     *
     * @param routerID
     * @return cost
     */
    unsigned get_cost(unsigned routerID) throw();

    /**
     * Gets a RIPRoutingEntry by routerID.
     * Throws exception if does not exist.
     *
     * @throw exception if no matching router is in forwarding table
     * @param routerID
     * @return RIPRoutingEntry
     */
    Route_table_entry get_entry(short routerID) throw();

};


#endif //RIP_RIP_H
