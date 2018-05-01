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
typedef std::chrono::milliseconds milliseconds;
typedef std::chrono::steady_clock::time_point time_point;
using std::chrono::steady_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

typedef struct Rip_header {
    /**
     * Header Struct for rip packets
     */
    byte command;           /* 1-REQUEST, 2-RESPONSE */
    byte version;          /* 1-RIPv1, 2-RIPv2 */
    short int routerID;    /* routerID */
} Header;

typedef struct Rip_entry {
    /**
     * RIP Entry struct for entries in packets.
     */
    short afi;
    short tag;
    int address;
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
    int destination{};
    int metric{};
    int nexthop{};
    time_point timeout_tmr;
    time_point garbage_tmr;
    byte route_changed{};       /*route change flag */
    byte marked_as_garbage{};   /* Marked as garbage flag */
} RTE;

typedef struct Timer_Intervals {
    int base;
    int timeout;
    int garbage_collection;
};

class Rip {
public:
    /**
     * Constructor.
     *
     * @param routerID
     * @param input_ports
     * @param outputs
     * @param timer
     */
    Rip(unsigned routerID, std::vector<unsigned> input_ports, std::vector<OutputInterface> outputs,
        unsigned timer = 30);

    /**
     * Main RIP Daemon loop.
     *
     */
    void run();

private:
    unsigned routerID;
    std::vector<unsigned> input_ports;
    std::vector<OutputInterface> interfaces;
    struct Timer_Intervals intervals;
    std::vector<Route_table_entry> routing_table;
    std::vector<rip_client_server::rip_server *> servers;
    std::vector<rip_client_server::rip_client *> clients;
    std::vector<int> output_ports;
    bool triggered;

    /**
     * Adds a header to a yet so far empty message.
     *
     * @param message - empty message of sufficient size to eventually hold the header and
     * all the RTEs.
     * @return the new message with a header included
     */
    char *add_header(char *message);

    /**
     * Adds another route entry to the routing table using information
     * from an RIP entry.
     *
     * @param entry - route entry.
     */
    void add_new_route(Rip_entry entry, int nextHop);

    /**
     * Adds another route entry to a message.
     *
     * @param message - message to add RTEs to
     * @param entry - Route Table Entry to glean information from for the rip entry
     * @return the new message with another RTE included at bottom
     */
    char *add_rip_entry(char *message, struct Route_table_entry entry);

    /**
     * Checks if update, timeout, or garbage collection timer
     * have timed out and takes action accordingly (e.g removing a
     * stale route entry if the garbage collection timer has expired).
     */
    void check_timers();

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
     * * Generates an RIP response message. Parameter isTriggered
     * has default value of false which implies a regular (timed)
     * update message. If isTriggered is True then a Triggered Update
     * Message will be generated.
     * @param msg buffer for the serialized message to be pu into
     * @param size size of the buffer
     * @param port_no int used to identify who the intended neighbor is for creating custom packet
     * @param isTriggered bool indicating whether this is a triggered event or not
     * @return char array containing the serialized packet header and entries for entry in router table
     */
    char *generate_response(char *msg, int port_no);

    /**
     * Get metric of peer router by its routerID.
     *
     * @param routerID
     * @return the metric
     */
    int get_cost(int routerID);

    /**
    * Gets a RIPRoutingEntry by routerID.
    * Throws exception if does not exist.
    *
    * @throw exception if no matching router is in forwarding table
    * @param routerID
    * @return RIPRoutingEntry
    */
    int get_entry(short routerID);

    /**
     * Removes stale route table entries (RTE) whose garbage collection
     * timers have expired.
     *
     * @param entry - RTE poised for removal
     */
    void handle_garbage_collection(Route_table_entry entry);

    /**
     * Initialize sockets for the sending of UDP packets. Available clients
     * can be accessed in the private instance variable clients.
     */
    void initialize_clients();

    /**
     * Initialize sockets for the receiving of UDP packets. Available servers
     * can be accessed in the private instance variable servers.
     */
    void initialize_servers();

    /**
     * Function sets up the routing table ot it's initial state using the
     * neighbors found in the config file and metrics
     */
    void initialize_table();

    /**
     * Function checks if the next hop to a destination is the router about to be messaged. This for the
     * split-horizon with poison-reverse implementation
     * @param entry: A route entry from the table
     * @param output: Output interface to determine which neighbor is about to be messaged
     * @return bool: True if the next hop is the router to be messaged
     */
    bool is_nextHop_neighbor(Route_table_entry entry, OutputInterface output);

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
     * Prints the routing table for the daemon in it's current state
     */
    void print_routing_table();

    /**
     * Prints a table entry.
     *
     * @param entry
     */
    void print_table_entry(Route_table_entry entry);

    /**
    * Checks each routing table entry included in a message that was received
    * from the network. If new routes are discovered it will attempt to
    * add them to its own routing table. If a more optimal alternative
    * path is discovered or an old path has been marked for expiration
    * then appropriate changes are made accordingly.
    *
    * @param packet - Rip_packet of data received from network
    */
    void process_response(Rip_packet *packet);


    /**
     * Reads an RIP entry to determine if any further action should be taken.
     *
     * @param entry - the RIP packey entry
     */
    void read_entry(Rip_entry entry, int originating_address);

    /**
    * Function sends update to neighboring routers once time limit is reached.
    * @param fdValue Value of one of the socket() call return values, will be used to assign sending port
    */
    void send_message(int fdValue, char *message, size_t size);

    /**
     * Function handles sending triggered updates
     */
    void send_update();

    /**
     * Initialize. Loads configuration settings, initalizes instance variables,
     * client and server sockets, and starts up the RIP daemon (todo)
     */
    void set_up(unsigned timer);

    /**
     * Marks entry as stale and initializes the garbage collection timer.
     *
     * @param Route Table Entry
     */
    void start_deletion_process(int index_of_entry);

    /**
     * Sends an update to each directly connect peer router.
     *
     * @param route_entry_index
     * @param metric
     */
    void update_route(int route_entry_index, int originating_address, int metric);

    /**
     * Validates packet is valid.
     *
     * @param packet
     * @return True (Valid) False otherwise
     */
    bool validate_packet(Packet packet);
};


#endif //RIP_RIP_H
