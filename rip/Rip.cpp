#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "Rip.h"
#include "RIPHeader.h"

Rip::Rip(unsigned _routerID, std::vector<unsigned> _input_ports, std::vector<OutputInterface> _outputs,
         unsigned _timer /* = 30 */) {
    unsigned routerID = _routerID;
    std::vector<unsigned> input_ports = _input_ports;
    std::vector<OutputInterface> outputs = _outputs;
    unsigned timer = _timer;
}


void Rip::initializeInputPorts() {
    for (unsigned sock: input_ports) {
        int s;
        if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("Cannot create a socket");
            return;
        }

        struct sockaddr_in socketAddress{};
        memset((char *)&socketAddress, 0, sizeof(socketAddress));
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        socketAddress.sin_port = static_cast<in_port_t>(sock);

        if (bind(s, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) < 0) {
            perror("Bind failed");
            return;
        }
        std::cout << "Successfully bound socket to port " << sock << std::endl;
    }
}


void Rip::initializeOutputPorts() {

}


void Rip::run() {
    initializeInputPorts();
}

const std::vector<unsigned int> &Rip::getInput_ports() const {
    return input_ports;
}
