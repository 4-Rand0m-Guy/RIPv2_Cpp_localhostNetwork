
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sstream>
#include <arpa/inet.h>
#include <cstdlib>
#include <sys/select.h>
#include <random>
#include <utility> #include <stdint.h>
#include "Rip.h"
#include "../config/ConsoleLogger.h"
#include "rip_client_server.h"

using Server = rip_client_server::rip_server;
using Client = rip_client_server::rip_client;
using Rip_error = rip_client_server::rip_client_server_runtime_error;

Rip::Rip(unsigned _routerID, std::vector<unsigned> _input_ports, std::vector<OutputInterface> _outputs,
         unsigned timer /* = 30 */) {
    routerID = _routerID;
    input_ports = std::move(_input_ports);
    interfaces = std::move(_outputs);
    init(timer);
    /*
     *
     * REST OF THIS FUNCTION IS TEST AND SHOULD BE DELETED OR REFACTORED INTO OTHER METHODS
     * */
    char received[DGRAM_SIZE];
    int max_fd = servers.at(0)->get_socket();
    fd_set sock_set;
    auto outer_timer = std::chrono::steady_clock::now();
    long time_elapsed;
    bool run = true;
    while(run) {
        struct timeval timeout = {0, 500000};
        FD_ZERO(&sock_set);
        for (Server* server: servers) {
            FD_SET(server->get_socket(), &sock_set);
            if (server->get_socket() > max_fd) {
                max_fd = server->get_socket();
            }
        }
        int activity = select(max_fd + 1, &sock_set, nullptr, nullptr, &timeout);
        if ((activity < 0) && (errno != EINTR)) {
            perror("Select error");
            run = false;
        } else if (activity == 0){          //timeout for select, no updates received
            //todo refine
            time_elapsed = duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                         outer_timer).count();
            if (time_elapsed > intervals.base * 1000) {
                std::cout << "Time elapsed since last update (ms)" << time_elapsed << std::endl;
                sendUpdate();
                outer_timer = std::chrono::steady_clock::now();
            }
        } else {                            //received a packet
            time_elapsed = duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                         outer_timer).count();
            //todo refine and implement RIP
            for (Server* server: servers) {
                if (FD_ISSET(server->get_socket(), &sock_set)) {
                    int bytes_received = server->recv(received, DGRAM_SIZE);
                    if (bytes_received > 0) {
                        std::cout << "bytes recv: " << bytes_received << std::endl;
                        Packet packet = deserialize_rip_message(received, bytes_received);
                        processPacket(&packet);
                        check_timers();
                    }
                }
            }
            if (time_elapsed > intervals.base * 1000) {
                std::cout << "Time elapsed since last update (ms)" << time_elapsed << std::endl;
                sendUpdate();
                outer_timer = std::chrono::steady_clock::now();
            }

        }
        /*int bytes_recv = server1->recv(received, DGRAM_SIZE);
        if (bytes_recv > 0) {
            std::cout << "bytes recv: " << bytes_recv << std::endl;
            Packet packet = deserialize_rip_message(received, bytes_recv);
        }

        if ((float( std::clock() - begin_time ) /  CLOCKS_PER_SEC) > 5.0) {
            std::cout << "sending: "<< std::endl;
            char message[size];
            generate_response(message, size);
            send_message(0, message, size);
            begin_time = std::clock();
        };*/
    }
}

void Rip::init(unsigned basetimer) {

    intervals.base = basetimer;
    intervals.timeout= basetimer * 4;
    intervals.garbage_collection = basetimer * 6;

    for (auto o: interfaces) {
        output_ports.push_back(o.port_number);
    }
    initializeServers();
    initializeClients();
    initializeTable();
    print_routing_table();
}

void Rip::initializeServers() {
    std::vector<unsigned>::iterator it;
    for (it = input_ports.begin(); it != input_ports.end(); ++it) {
        try {
            Server *server = new Server(HOST, *it);
            servers.push_back(server);
        } catch (Rip_error &e) {
            it = input_ports.erase(it);
            perror(e.what());
        }
    }
};

void Rip::initializeClients() {
    std::vector<int>::iterator it;
    for (it = output_ports.begin(); it != output_ports.end(); ++it) {
        try {
            Client *client = new Client(HOST, *it);
            clients.push_back(client);
        } catch (Rip_error &e) {
            it = output_ports.erase(it);
            perror(e.what());
        }
    }
}

void Rip::initializeEventFd() {
//    int triggered = eventfd
}

//setup the table using neighbors from initial config file
void Rip::initializeTable() {
    Route_table_entry entry;
    entry.destination = routerID;
    entry.metric = 0;
    entry.nexthop = routerID;
    entry.timeout_tmr = steady_clock::now();
    routingTable.push_back(entry);
    for (auto out: interfaces) {
        entry.destination = out.id;
        entry.metric = out.metric;
        entry.nexthop = out.id;
        entry.route_changed = 0;
        entry.timeout_tmr = steady_clock::now();
        routingTable.push_back(entry);
    }
}

/* SENDS A SINGLE MESSAGE TO A UDP SOCKET */
void Rip::send_message(int fd, char* message, size_t size) {
    Client* client = clients.at(static_cast<unsigned long>(fd));
    int bytes_sent = client->send(message, size);
    if (bytes_sent < 0) {
        fprintf(stderr, "socket(%i) failed: %s\n", fd, strerror(errno));
    } /*else {
        printf("%i bytes sent...\n", bytes_sent); // todo: get router id by fd
    }*/
}

char* Rip::generate_response(char* message, int size, bool isTriggered) {
    char* p_message = message;
    p_message = add_header(p_message);
    for (Route_table_entry entry : routingTable) {
        struct Route_table_entry temp{};
        bool is_next_hop = false;
        for (OutputInterface out: interfaces) {
            if (nextHopIsRouter(entry, out)) {              //The metric for neighbour needs to be 16 in this case
                is_next_hop = true;
            }
        }
        if (is_next_hop) {
            temp.metric = 16;
        }
        p_message = add_rip_entry(p_message, temp);
    }
}

char* Rip::add_header(char* message) {
    Rip_header header{};
    header.command = '2'; // REQUEST unsupported
    header.version = RIP_VERSION;
    header.routerID = static_cast<short>(routerID);
    memcpy(message, (void *)&header, sizeof(header));
    message += sizeof(header);
    return message;
}

char* Rip::add_rip_entry(char *message, struct Route_table_entry entry) {
    Rip_entry rentry{};
    rentry.afi = 0;
    rentry.tag = 0;
    rentry.address = entry.destination;
    rentry.subnetmask = 0;
    rentry.nextHop = entry.nexthop;
    rentry.metric = entry.metric;
    char* tempmessage = message;
    memcpy(message, (void *)&rentry, sizeof(rentry));
    tempmessage += sizeof(rentry);
    return tempmessage;
}

void Rip::add_new_route(Rip_entry entry, int nextHop) {
    Route_table_entry RTE;
    RTE.destination = entry.address;
    RTE.metric = entry.metric;
    RTE.nexthop = nextHop;
    RTE.timeout_tmr = steady_clock::now();
    RTE.marked_as_garbage = 0;
    RTE.route_changed = 0;
    routingTable.push_back(RTE);
    printf("New route to %i via %i has been added...", RTE.destination, RTE.nexthop);
    // todo signal to trigger an update
}

Rip_packet Rip::deserialize_rip_message(char* message, int bytes_received) {
    Packet packet;
    packet.header = *(Header*) message;
    message += HEADER_SIZE;
    int entry_size = RTE_SIZE;
    //print_header(packet.header);
    for (int current_byte = HEADER_SIZE; current_byte < bytes_received; current_byte+=RTE_SIZE) {
        Rip_entry rte = *(Rip_entry*) message;
        message += entry_size;
        packet.entries.push_back(rte);
        //print_entry(rte);
    }
    return packet;
}

void Rip::check_timers() {
    time_point current_time = steady_clock::now();
    for (Route_table_entry entry : routingTable) {
        long long time_elapsed;
        if (entry.destination == routerID) {
            continue;
        }
        if (entry.marked_as_garbage != 1) {
            time_elapsed = duration_cast<seconds>(current_time - entry.timeout_tmr).count();
            std::cout << "timeout elapse: " <<  time_elapsed << std::endl;
            if (time_elapsed > intervals.timeout) {
                start_deletion_process(entry);
            }
        } else {
            time_elapsed = duration_cast<seconds>(current_time - entry.garbage_tmr).count();
            std::cout << "garbage elapse: " <<  time_elapsed << std::endl;
            if (time_elapsed > intervals.garbage_collection) {
                handle_garbage_collection(entry);
            }
        }
    }
}

void Rip::handle_garbage_collection(Route_table_entry entry) {
    std::vector<Route_table_entry>::iterator it;
    for (it = routingTable.begin(); it != routingTable.end(); ++it) {
        if (entry.destination == (*it).destination && entry.marked_as_garbage == 1) {
            routingTable.erase(it);
            printf("Stale route to %i via %i has been pruned...\n", entry.destination, entry.nexthop);
        }
    }
}

void Rip::print_header(struct Rip_header header) {
    printf("[ RIP Packer Header ]\n");
    printf("| command: %c | ", header.command);
    printf("version: %c | ", header.version);
    printf("routerID: %hi |", header.routerID);
}

void Rip::print_entry(struct Rip_entry entry) {
    printf("\n[ RIP Route Entry ]\n");
    printf("| afi: %hi ", entry.afi);
    printf("| tag: %hi |\n", entry.tag);
    printf("| address: %i    |\n", entry.address);
    printf("| subnetmask: %i   |\n", entry.subnetmask);
    printf("| nextHop: %i      |\n", entry.nextHop);
    printf("| metric: %i       |\n", entry.metric);
}

void Rip::print_routing_table() {
    printf("\nRouting table for Daemon: %i\n", routerID);
    printf("--------------------------------------------\n");
    int i = 1;
    for (Route_table_entry entry: routingTable) {
        print_table_entry(entry);
        i++;
    }
    printf("--------------------------------------------\n");
}

void Rip::print_table_entry(Route_table_entry entry) {
    double time_left;
    std::chrono::duration<double, std::milli> time_elapsed = (steady_clock::now() - entry.timeout_tmr);
    printf("| destination: %i\n", entry.destination);
    printf("| metric: %i\n", entry.metric);
    printf("| nextHop: %i\n", entry.nexthop);
    time_left = static_cast<double>(intervals.timeout) - (time_elapsed.count()/ 1000.0);
    printf("| timeout in %.2lf\n", time_left);
    if (entry.garbage_tmr.time_since_epoch().count() <= 0) {
        printf("| garbage timeout: N/A\n");
    } else {
        time_elapsed = steady_clock::now() - entry.garbage_tmr;
        time_left = static_cast<double>(intervals.garbage_collection) - (time_elapsed.count()/ 1000.0);
        printf("| garbage timeout in:%.2lf\n", time_left);
    }
    if (entry.route_changed == 0) {
        printf("| route changed: NO\n");
    } else {
        printf("| route changed: YES\n");
    }
    if (entry.marked_as_garbage == 0) {
        printf("| marked as garbage: NO\n");
    } else {
        printf("| marked as garbage: YES\n");
    }
    ;
    printf("**************************\n");
}

Route_table_entry Rip::get_entry(short routerID) noexcept {
    std::cout << "Entered get_entry" << std::endl;
    for (auto route : routingTable) {
        if (routerID == route.destination) {
            std::cout << "get_entry entry found" << std::endl;
            return route;
        }
    }
    std::cout << "get_entry entry not found" << std::endl;
    throw(0);
}

int Rip::get_cost(int routerID) noexcept {
    for (OutputInterface iface : interfaces) {
        if (routerID == iface.id) {
            return iface.metric;
        }
    }
    throw(0);
}

bool Rip::nextHopIsRouter(Route_table_entry entry, OutputInterface output) {
    bool value =false;
    if (entry.nexthop == output.id) {
    value = true;
    }
    return value;
}

void Rip::processPacket(Packet *packet) {
    std::cout << "Problem is not at start of process packet" << std::endl;
    if (validate_packet(*packet)) {
        for (Entry entry: packet->entries) {
            entry.metric = std::min(entry.metric + get_cost(packet->header.routerID), INFINITY);
            read_entry(entry, packet->header.routerID);
        }
    } else {
        std::cout << "Invalid packet. Dropped." << std::endl;
    }
}

void Rip::read_entry(Rip_entry rip_route, int originating_address) {
    std::cout << "Entered read_entry" << std::endl;
    try {
        std::cout << "Entered read_entry try block" << std::endl;
        Route_table_entry RTE = get_entry(static_cast<short>(rip_route.address));
        if (RTE.metric != INFINITY) {
            if (RTE.nexthop == originating_address) { // re-init timeout_tmr
                if (RTE.metric != rip_route.metric) { // same route different metric
                    std::cout << "read_entry same route different metric" << std::endl;
                    if (rip_route.metric >= INFINITY) {
                        std::cout << "read_entry same route different metric: dead route" << std::endl;
                        start_deletion_process(RTE); // route has been deleted abroad
                    } else {
                        std::cout << "read_entry same route different metric: not dead" << std::endl;
                        update_route(RTE, rip_route); // same route better or worse metric
                    }
                } else {
                    std::cout << "read_entry everything is the same" << std::endl;
                    RTE.timeout_tmr = steady_clock::now(); // SAME EVERYTHING
                }
            } else {
                if (RTE.metric > rip_route.metric) {
                    std::cout << "read_entry different route: better metric" << std::endl;
                    update_route(RTE, rip_route); // different route better metric
                } else if (RTE.metric == rip_route.metric) {
                    // optional functionality, best just ignore
                }
            }
        } else {
            if (rip_route.metric != INFINITY) { // ROUTE HAS BEEN REVIVED
                if (rip_route.nextHop == originating_address) {
                    // ROUTE RESTORED
                    update_route(RTE, rip_route);
                } else {
                    // New route to new destination restored
                    update_route(RTE, rip_route);
                }
            }
        }
    } catch (int e) { // BRAND NEW ROUTE
        std::cout << "Caught exception thrown by get_entry" << std::endl;
        if (rip_route.metric < INFINITY) {
            add_new_route(rip_route, originating_address);
        }
    }
}

void Rip::update_route(Route_table_entry RTE, Rip_entry rip_entry) {
    RTE.marked_as_garbage = 0;
    RTE.route_changed = 0;
    RTE.nexthop = rip_entry.nextHop;
    RTE.timeout_tmr = steady_clock::now();
    RTE.metric = rip_entry.metric;
    RTE.nexthop = rip_entry.nextHop;
    RTE.timeout_tmr = steady_clock::now();
    RTE.garbage_tmr = {};
    printf("Route to %i via %i has been updated...\n", RTE.destination, RTE.nexthop);
    };

bool Rip::validate_packet(Packet packet) {
    std::cout << "Problem is not in validate packet" << std::endl;
    for (auto ifaces : interfaces) {
        if (packet.header.routerID == ifaces.id) {
            return true;
        }
    }
    return false;
}

void Rip::sendUpdate() {
    for (int i = 0; i < clients.size(); i++ ) {
        size_t size = (routingTable.size() * RTE_SIZE) + HEADER_SIZE; //exclude route to neighbor router
        char message[size];
        generate_response(message, static_cast<int>(size));
        send_message(i, message, size);
    }
    std::cout << "Updated" << std::endl;
}

void Rip::start_deletion_process(Route_table_entry RTE) {
    RTE.garbage_tmr = steady_clock::now();
    RTE.metric = INFINITY;
    RTE.route_changed = 1;
    RTE.marked_as_garbage = 1;
    printf("Route to %i via %i has gone stale and been marked as garbage...\n", RTE.destination, RTE.nexthop);
}


