#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sstream>
#include <chrono>
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
         unsigned _timer /* = 30 */) {

    routerID = _routerID;
    input_ports = _input_ports;
    outputs = _outputs;
    timer = _timer;
    init();
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
    while(run) {
        timeout.tv_sec = timer;
        timeout.tv_usec = 0;
        size_t size = (routingTable.size() * RTE_SIZE) + HEADER_SIZE;
        char message[size];
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
        } else if (activity == 0){
            //todo timout occured
            std::cout << "Timed out" << std::endl;
            for (Client* client: clients) {
                generate_response(message, static_cast<int>(size));
                send_message(client->get_port(), message, size);
            }
        } else {
            std::cout << "receive something" << std::endl;
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

void Rip::init() {
    for (auto o: outputs) {
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
    for (auto out: outputs) {
        Route_table_entry entry;
        entry.destination = out.id;
        entry.metric = out.metric;
        entry.nexthop = out.id;
        entry.routechanged = 0;
        routingTable.push_back(entry);
    }
}

/* SENDS A SINGLE MESSAGE TO A UDP SOCKET */
void Rip::send_message(int fd, char* message, size_t size) {
    Client* client = clients.at(fd);
    int bytes_sent = client->send(message, size);
    if (bytes_sent < 0) {
        fprintf(stderr, "socket(%i) failed: %s\n", fd, strerror(errno));
    } else {
        printf("%i bytes sent...\n", bytes_sent); // todo: get router id by fd
    }
}


char* Rip::generate_response(char* message, int size, bool isTriggered) {
    // todo implement isTrigger functionality i.e. routesChanged
    char* p_message = message;
    p_message = add_header(p_message);
    for (Route_table_entry entry : routingTable) {
        p_message = add_RTE(p_message, entry);
    }
}

char* Rip::add_header(char *message) {
    Rip_header header;
    header.command = '2'; // REQUEST unsupported
    header.version = RIP_VERSION;
    header.routerID = routerID;
    memcpy(message, (void *)&header, sizeof(header));
    message += sizeof(header);
    return message;
}

char* Rip::add_RTE(char *message, struct Route_table_entry entry) {
    Rip_entry rentry;
    rentry.afi = 0;
    rentry.tag = 0;
    rentry.ipaddress = entry.destination;
    rentry.subnetmask = 0;
    rentry.nextHop = entry.nexthop;
    rentry.metric = entry.metric;
    char* tempmessage = message;
    memcpy(message, (void *)&rentry, sizeof(rentry));
    tempmessage += sizeof(rentry);
    return tempmessage;
}

Rip_packet Rip::deserialize_rip_message(char *message, int bytes_received) {
    Packet packet;
    packet.header = *(Header*) message;
    message += HEADER_SIZE;
    int entry_size = RTE_SIZE;
    print_header(packet.header);
    for (int current_byte = HEADER_SIZE; current_byte < bytes_received; current_byte+=RTE_SIZE) {
        Rip_entry rte = *(Rip_entry*) message;
        message += entry_size;
        packet.entries.push_back(rte);
        print_entry(rte);
    }
    return packet;
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
    printf("| ipaddress: %i    |\n", entry.ipaddress);
    printf("| subnetmask: %i   |\n", entry.subnetmask);
    printf("| nextHop: %i      |\n", entry.nextHop);
    printf("| metric: %i       |\n", entry.metric);
}

//Run function, handle the running of the daemon, events etc
//void Rip::run() {
//    std::cout << "Daemon " << routerID << " running" << std::endl;
//    int max_sd = 0; //max socket address (highest port value in the set of ports to listen to)
//    int activity;
//    struct timeval timeout{};
//    fd_set readfds; //set of file descriptors to listen to sockets
//    bool run = true;
//    std::vector<unsigned int> fdArray;
//    try {
//        fdArray = initializeInputPorts();
//    } catch (std::invalid_argument& e) {
//        std::cout << e.what() << std::endl;
//        run = false;
//    } catch (std::exception &e){
//        std::cout << "Get rekt by this unknown bug" << std::endl;
//        run = false;
//    }
//    //enter the infinite loop
//    auto t1 = std::chrono::high_resolution_clock::now();
//    while (run) {
//        timeout.tv_sec = timer;
//        timeout.tv_usec = 0;
//        FD_ZERO(&readfds);
//        //Add the current input sockets to the set of sockets to track for the select() function
//        for (auto sock: fdArray) {
//            FD_SET(sock, &readfds);
//            if (sock > max_sd) {
//                max_sd = sock;
//            }
//        }
//        activity = select(max_sd + 1, &readfds, nullptr, nullptr, &timeout);
//        if ((activity < 0) && (errno != EINTR)) { //something has gone wrong
//            perror("Select error");
//            run = false;
//        } else if (activity == 0) { //timeout reached with no activity, send periodic update
//            try {
//                sendUpdate(fdArray[0]);
//                t1 = std::chrono::high_resolution_clock::now();
//            } catch (std::invalid_argument &e) {
//                std::cout << e.what() << std::endl;
//                run = false;
//            }
//        } else { //some kind of activity has occurred
//            //have we recently sent an update?
//            for (auto fd: fdArray) {
//                unsigned char *data;
//                if (FD_ISSET(fd, &readfds)) {
//                    data = receive(fd);
//                }
//            }
//            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t1)
//                    .count() >= timer * 1000) {
//                try {
//                    sendUpdate(fdArray[0]);
//                } catch (std::invalid_argument &e) {
//                    std::cout << e.what() << std::endl;
//                    run = false;
//                }
//                t1 = std::chrono::high_resolution_clock::now();
//            }
//        }
//    }
//}

//Function sends updates to neighbor routers
//void Rip::sendUpdate(int fdValue, char* message) {
//    struct sockaddr_in sendingSocket{};
//    for (auto port: outputs) {
//        RIPHeader hdr = createHeader();
//        RIPPacket packet = RIPPacket(&hdr);
//        for (RIPRouteEntry entry: forwardingTable) {
//            RIPRouteEntry temp = entry;
//            if (nextHopIsRouter(temp, port)) {
//                temp.setMetric(16);
//            }
//            packet.addRoute(temp);
//        }
//        std::stringbuf buff;
//        std::ostream stream(&buff);
//        packet.serialize(stream);
//        memset((char *) &sendingSocket, 0, sizeof(sendingSocket));
//        sendingSocket.sin_family = AF_INET;
//        sendingSocket.sin_port = static_cast<in_port_t>(port.port_number);
//        if (inet_aton("127.0.0.1", &sendingSocket.sin_addr) == 0) {
//            perror("inet_aton failed");
//        }
//
//        struct rip_header rh;
//        struct rip_header rh2;
//
//
//        rh.command = 1;
//        rh.version = 2;
//        rh._zero = 3;
//
//        memset(message, '\0', 1024);
//        message = RIPPacket::add_rip_header_to_packet(message, rh);
//        RIPPacket::decode_rip_message(message, &rh2);
//
//        if (sendto(fdValue, &stream, 512, 0, reinterpret_cast<const sockaddr *>(&sendingSocket), sizeof
//        (sendingSocket)) == -1) {
//            perror("Sending packet failed");
//            throw std::invalid_argument("Most likely something wrong with your fileDescriptors");
//        } else {
//            std::cout << "Message sent" << std::endl;
//        }
//    }
//}

unsigned Rip::get_cost(unsigned routerID) throw() {
    for (OutputInterface iface : outputs) {
        if (routerID == iface.id) {
            return iface.metric;
        }
    }
    throw;
}

Route_table_entry Rip::get_entry(short routerID) throw() {
    for (auto route : routingTable) {
        if (routerID == route.destination) {
            return route;
        }
    }
    throw;
}

bool nextHopIsRouter(Route_table_entry entry, OutputInterface output) {
    bool value =false;
    if (entry.nexthop == output.id) {
    value = true;
    }
    return value;
}


