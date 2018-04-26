#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sstream>
#include <chrono>
#include "Rip.h"
#include "RIPHeader.h"

Rip::Rip(unsigned _routerID, std::vector<unsigned> _input_ports, std::vector<OutputInterface> _outputs,
         unsigned _timer /* = 30 */) {
    routerID = _routerID;
    input_ports = _input_ports;
    outputs = _outputs;
    timer = _timer;
}

//Initialize the input ports (bind sockets etc)
void Rip::initializeInputPorts() {
    for (auto sock: input_ports) {
        int s;
        if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("Cannot create a socket");
            return;
        }

        struct sockaddr_in socketAddress{};
        memset((char *) &socketAddress, 0, sizeof(socketAddress));
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        socketAddress.sin_port = static_cast<in_port_t>(sock);

        if (bind(s, (struct sockaddr *) &socketAddress, sizeof(socketAddress)) < 0) {
            std::cout << "Binding socket: " << sock << std::endl;
            perror("Bind failed");
            continue;
        } else {
            std::cout << "Successfully bound socket to port " << sock << std::endl;
        }

    }
}

//Run function, handle the running of the daemon, events etc
void Rip::run() {
    std::cout << "Daemon running" << std::endl;
    int max_sd = 0; //max socket address (highest port value in the set of ports to listen to)
    int activity;
    struct timeval timeout{};
    fd_set readfds; //set of file descriptors to listen to sockets
    initializeInputPorts();
    bool run = true;

    //enter the infinite loop
    while (run) {
        timeout.tv_sec = timer;
        timeout.tv_usec = 0;
        FD_ZERO(&readfds);
        //Add the current input sockets to the set of sockets to track for the select() function
        for (auto sock: input_ports) {
            FD_SET(sock, &readfds);
            if (sock > max_sd) {
                max_sd = sock;
            }
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        activity = select(max_sd + 1, &readfds, NULL, NULL, &timeout);
        if ((activity < 0) && (errno != EINTR)) {
            perror("Select error");
        } else if (activity == 0) {
            std::cout << "Timeout reached, no updates" << std::endl;
            continue;
        } else {
            std::cout << "Implement update event please" << std::endl;
        }

    }

}


const std::vector<unsigned int> &Rip::getInput_ports() const {
    return input_ports;
}
