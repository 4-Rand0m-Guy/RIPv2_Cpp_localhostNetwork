#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sstream>
#include <chrono>
#include <arpa/inet.h>
#include <cstdlib>
#include <random>
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
std::vector<unsigned int> Rip::initializeInputPorts() {
    std::vector<unsigned int> fdArray;
    for (auto sock: input_ports) {
        unsigned int s;
        if ((s = static_cast<unsigned int>(socket(AF_INET, SOCK_DGRAM, 0))) < 0) {
            perror("Cannot create a socket");
            throw std::exception();
        } else {
            fdArray.push_back(s);
        }

        struct sockaddr_in socketAddress{};
        memset((char *) &socketAddress, 0, sizeof(socketAddress));
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        socketAddress.sin_port = static_cast<in_port_t>(sock);

        if (bind(s, (struct sockaddr *) &socketAddress, sizeof(socketAddress)) < 0) {
            perror("Bind failed");
            throw std::invalid_argument("Something went wrong binding, port reserved or in use");
        } else {
            std::cout << "Successfully bound socket to port " << sock << std::endl;
        }
    }
    return fdArray;
}

//Run function, handle the running of the daemon, events etc
void Rip::run() {
    std::cout << "Daemon running" << std::endl;
    int max_sd = 0; //max socket address (highest port value in the set of ports to listen to)
    int activity;
    struct timeval timeout{};
    fd_set readfds; //set of file descriptors to listen to sockets
    bool run = true;
    std::vector<unsigned int> fdArray;
    try {
        fdArray = initializeInputPorts();
    } catch (std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
        run = false;
    } catch (std::exception &e){
        std::cout << "Get rekt by this unknown bug" << std::endl;
        run = false;
    }
    //enter the infinite loop
    while (run) {
        timeout.tv_sec = timer;
        timeout.tv_usec = 0;
        FD_ZERO(&readfds);
        //Add the current input sockets to the set of sockets to track for the select() function
        for (auto sock: fdArray) {
            FD_SET(sock, &readfds);
            if (sock > max_sd) {
                max_sd = sock;
            }
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        activity = select(max_sd + 1, &readfds, nullptr, nullptr, &timeout);
        if ((activity < 0) && (errno != EINTR)) { //something has gone wrong
            perror("Select error");
            run = false;
        } else if (activity == 0) { //timeout reached with no activity, send periodic update
            try {
                sendUpdate(fdArray[0]);
            } catch (std::invalid_argument &e) {
                std::cout << e.what() << std::endl;
                run = false;
            }
        } else { //some kind of activity has occurred
            //have we recently sent an update?
            for (auto fd: fdArray) {
                if (FD_ISSET(fd, &readfds)) {
                    receive(fd);
                }
            }
            if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t1)
                    .count() >= randomTimeGenerator()) {
                try {
                    sendUpdate(fdArray[0]);
                } catch (std::invalid_argument &e) {
                    std::cout << e.what() << std::endl;
                    run = false;
                }
            }
        }
    }
}

//retrieve input ports, for debug purposes
const std::vector<unsigned int> &Rip::getInput_ports() const {
    return input_ports;
}

//Function sends updates to neighbor routers
void Rip::sendUpdate(int fdValue) {
    struct sockaddr_in sendingSocket{};
    auto * message = const_cast<char *>("Updating neighbors");

    for (auto port: outputs) {
        memset((char *) &sendingSocket, 0, sizeof(sendingSocket));
        sendingSocket.sin_family = AF_INET;
        sendingSocket.sin_port = static_cast<in_port_t>(port.port_number);
        if (inet_aton("127.0.0.1", &sendingSocket.sin_addr) == 0) {
            perror("inet_aton failed");
        }

        if (sendto(fdValue, message, 512, 0, reinterpret_cast<const sockaddr *>(&sendingSocket), sizeof
        (sendingSocket)) == -1) {
            perror("Sending packet failed");
            throw std::invalid_argument("Most likely something wrong with your fileDescriptors");
        } else {
            std::cout << "Message sent" << std::endl;
        }
    }
}

//setup the table using neighbors from initial config file
void Rip::initializeTable() {

}

double Rip::randomTimeGenerator() {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.8 * timer, 1.2 * timer);
    double diceRoll = distribution(generator);
    std::cout << diceRoll << std::endl;
    return diceRoll;
}

void Rip::receive(unsigned int fd) {
    struct sockaddr_in senderAddr{};
    socklen_t addrLen = sizeof(senderAddr);
    ssize_t messageLen;
    unsigned char buff[512];

    messageLen = recvfrom(fd, buff, 512, 0, (struct sockaddr*)&senderAddr, &addrLen);
    std::cout << "Received " << messageLen << " bytes" << std::endl;
    if (messageLen > 0) {
        buff[messageLen] = 0;
        std::cout << "Received message: " << buff << std::endl;
    }

}
