#ifndef RIP_RIP_H
#define RIP_RIP_H
#define DGRAM_SIZE 504
#define HOST "127.0.0.1"
#define RIP_VERSION '2'
#define HEADER_SIZE 4
#define RTE_SIZE 20
#define INFINITY 16

#include <map>
#include <vector>
#include <chrono>
#include "../config/OutputInterface.h"
#include "rip_client_server.h"



typedef char byte;

typedef std::chrono::seconds seconds;
typedef std::chrono::steady_clock::time_point time_point;
using std::chrono::steady_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


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

    time_point timeout_tmr;
    time_point garbage_tmr;

    byte route_changed;		/*route change flag */
    byte marked_as_garbage; /* Marked as garbage flag */
} RTE;

typedef struct Timer_Intervals {
    int base;
    int timeout;
    int garbage_collection;
};

typedef struct Timers {
    time_point update;
    time_point check;
};

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
    std::vector<OutputInterface> interfaces;
    struct Timer_Intervals intervals;
    std::vector<Route_table_entry> routingTable;
    std::vector<rip_client_server::rip_server*> servers;
    std::vector<rip_client_server::rip_client*> clients;
    std::vector<int> output_ports;

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
     * Checks if update, timeout, or garbage collection timer
     * have timed out and takes action accordingly (e.g removing a
     * stale route entry if the garbage collection timer has expired).
     */
    void check_timers();

    /**
     * Initialize. Loads configuration settings, initalizes instance variables,
     * client and server sockets, and starts up the RIP daemon (todo)
     */
    void init(unsigned timer);

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

    void initializeTimeout();

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
     * Removes stale route table entries (RTE) whose garbage collection
     * timers have expired.
     *
     * @param entry - RTE poised for removal
     */
    void handle_garbage_collection(Route_table_entry entry);

    /**
     * Marks entry as stale and initializes garbage collection timer.
     *
     * @param entry
     */
    void handle_timeout_expiry(Route_table_entry entry);

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
     * Get metric of peer router by its routerID.
     *
     * @param routerID
     * @return the metric
     */
    int get_cost(int routerID) throw();

    /**
    * Gets a RIPRoutingEntry by routerID.
    * Throws exception if does not exist.
    *
    * @throw exception if no matching router is in forwarding table
    * @param routerID
    * @return RIPRoutingEntry
    */
    Route_table_entry get_entry(short routerID) throw();

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
     * Function checks if the next hop to a destination is the router about to be messaged. This for the
     * split-horizon with poison-reverse implementation
     * @param entry: A route entry from the table
     * @param output: Output interface to determine which neighbor is about to be messaged
     * @return bool: True if the next hop is the router to be messaged
     */
    bool nextHopIsRouter(Route_table_entry entry, OutputInterface output);

    /**
     * Function goes through the received packet, checks what routes it contains, updates the daemons table as
     * necessary.
     * @param packet
     */
    void processPacket(Rip_packet* packet);


    /**
     * Reads an RIP entry to determine if any further action should be taken.
     *
     * @param entry - the RIP packey entry
     */
    void read_entry(Rip_entry entry);

    /**
     * Validates packet is valid.
     *
     * @param packet
     * @return True (Valid) False otherwise
     */
    bool validate_packet(Packet packet);
};


#endif //RIP_RIP_H
