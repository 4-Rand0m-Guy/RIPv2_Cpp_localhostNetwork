#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sstream>
#include <arpa/inet.h>
#include <cstdlib>
#include <sys/select.h>
#include <random>
#include <stdint.h>
#include "Rip.h"
#include "../config/ConsoleLogger.h"
#include "rip_client_server.h"

using Server = rip_client_server::rip_server;
using Client = rip_client_server::rip_client;
using Rip_error = rip_client_server::rip_client_server_runtime_error;

Rip::Rip(unsigned _routerID, std::vector<unsigned> _input_ports, std::vector<OutputInterface> _outputs,
         unsigned timer /* = 30 */) {

    routerID = _routerID;
    input_ports = _input_ports;
    interfaces = _outputs;

    init(timer);
    std::cout << "Running daemon ID: " << routerID << std::endl;
    /*
     *
     * REST OF THIS FUNCTION IS TEST AND SHOULD BE DELETED OR REFACTORED INTO OTHER METHODS
     * */

    char received[DGRAM_SIZE];
    int max_fd = servers.at(0)->get_socket();
    fd_set sock_set;
    struct timeval timeout{};
    bool run = true;
    auto outer_timer = std::chrono::steady_clock::now();
    while(run) {
        timeout.tv_sec = 0;
        timeout.tv_usec = 5000000;
        auto time_elapsed = duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                     outer_timer).count();
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
        } else {
            time_elapsed = duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                         outer_timer).count();
            //todo refine and implement RIP
            for (Server* server: servers) {
                if (FD_ISSET(server->get_socket(), &sock_set)) {
                    int bytes_received = server->recv(received, DGRAM_SIZE);
                    if (bytes_received > 0) {
                        std::cout << "bytes recv: " << bytes_received << std::endl;
                        Packet packet = deserialize_rip_message(received, bytes_received);
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
}

void Rip::initializeServers() {
    std::vector<unsigned>::iterator it;
    for (it = input_ports.begin(); it != input_ports.end(); ++it) {
        try {
            Server *server = new Server(HOST, *it);
            servers.push_back(server);
        } catch (Rip_error e) {
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
        } catch (Rip_error e) {
            it = output_ports.erase(it);
            perror(e.what());
        }
    }
}

//setup the table using neighbors from initial config file
void Rip::initializeTable() {
    for (auto out: interfaces) {
        Route_table_entry entry;
        entry.destination = out.id;
        entry.metric = out.metric;
        entry.nexthop = out.id;
        entry.route_changed = 0;
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
        bool is_dest = false;
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
    Rip_header header;
    header.command = '2'; // REQUEST unsupported
    header.version = RIP_VERSION;
    header.routerID = routerID;
    memcpy(message, (void *)&header, sizeof(header));
    message += sizeof(header);
    return message;
}

char* Rip::add_rip_entry(char *message, struct Route_table_entry entry) {
    Rip_entry rentry;
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

void Rip::add_route_table_entry(Rip_entry entry, int nextHop) {
    Route_table_entry RTE;
    RTE.destination = entry.address;
    RTE.metric = entry.metric;
    RTE.nexthop = nextHop;
    RTE.timeout_tmr = steady_clock::now();
    RTE.route_changed = 1;
    RTE.marked_as_garbage = 0;
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
                handle_timeout_expiry(entry);
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

void Rip::handle_timeout_expiry(Route_table_entry entry) {
    entry.garbage_tmr = steady_clock::now();
    entry.marked_as_garbage = 1;
    entry.route_changed = 1;
    entry.metric = INFINITY;
    printf("Route to %i via %i has gone stale and been marked as garbage...\n", entry.destination, entry.nexthop);
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

Route_table_entry Rip::get_entry(short routerID) throw() {
    for (auto route : routingTable) {
        if (routerID == route.destination) {
            return route;
        }
    }
    throw(0);
}

int Rip::get_cost(int routerID) throw() {
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
    if (validate_packet(*packet)) {
        for (Entry entry: packet->entries) {
            entry.metric = std::min(entry.metric + get_cost(packet->header.routerID), INFINITY);
            read_entry(entry, packet->header.routerID);
        }
    } else {
        std::cout << "Invalid packet. Dropped." << std::endl;
    }
}

void Rip::read_entry(Rip_entry entry, int originating_address) {
    try {
        Route_table_entry RTE = get_entry(entry.address);
        /**
         *
         *
         * //todo: this entire code block
         *
         *
         */
    } catch (int e) {
        if (entry.metric < INFINITY) {
            add_route_table_entry(entry, originating_address);
        }
    }
}


bool Rip::validate_packet(Packet packet) {
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


