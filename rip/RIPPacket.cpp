#include <sstream>
#include <iostream>
#include "RIPPacket.h"

RIPPacket::RIPPacket(unsigned char* data, int length) {
    message = data;
    cur_len = length;
    deserialize(data, length);
}

RIPPacket::RIPPacket(RIPHeader* _header) {
    header = _header;
}

void RIPPacket::deserialize(unsigned char *outBuffer, int length) {
    unsigned char hdr_arr[4] = {outBuffer[0],outBuffer[1],outBuffer[2],outBuffer[3]};
    RIPHeader hdr = RIPHeader(hdr_arr);
    *header = hdr;
    if (length > 4) {
        unsigned char entry[20];
        for (auto i = 4; i < 504; i +=4) {
            entry[i%20] = outBuffer[i];
            if ((i-4)%20 == 0 && i > 4) {
                addRoute(entry);
            }
        }
    }
}

void RIPPacket::addRoute(unsigned char * rte) {
    routes.push_back(RIPRouteEntry(rte));
    unsigned char result[cur_len + 20];
    std::copy(message, message + cur_len, result);
    std::copy(rte, rte + 20, result + cur_len);
    cur_len += 20;
}

void RIPPacket::addRoute(RIPRouteEntry rte) {
    unsigned char route[20];
    rte.serialize(route);
    addRoute(route);
}

std::string RIPPacket::toString() {
    std::stringstream fmt;
    fmt << "+______________________________________+\n"
    << header->toString();
    for (RIPRouteEntry route : routes) {
        fmt << "+--------------------------------------+" << '\n'
        << route.toString() << "\n"
        << "+--------------------------------------+" << '\n';
    }
    fmt << std::endl;
    std::string s = fmt.str();
    return s;
}