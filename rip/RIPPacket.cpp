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
    cur_len = 4;
    _header->serialize(message);
}

RIPPacket::RIPPacket(RIPHeader *_header, std::vector<RIPRouteEntry> rtes) {
    header = _header;
    cur_len = 4 + rtes.size();
    _header->serialize(message);
    for (RIPRouteEntry rte : rtes) {
        addRoute(rte);
    }
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

void RIPPacket::serialize(unsigned char *inBuffer) {
    inBuffer = message;
}

void RIPPacket::addRoute(RIPRouteEntry rte) {
    unsigned char route[20];
    rte.serialize(route);
    addRoute(route);
}

void RIPPacket::addRoute(unsigned char * rte) {
    routeEntries.push_back(RIPRouteEntry(rte));
    unsigned char result[cur_len + 20];
    std::copy(message, message + cur_len, result);
    std::copy(rte, rte + 20, result + cur_len);
    cur_len += 20;
}


std::string RIPPacket::toString() {
    std::cout << "1. " << header << std::endl;
    std::cout << "2. " << header->toString() << std::endl;
    std::stringstream fmt;
    fmt << "+______________________________________+\n"
    << header->toString();
    for (RIPRouteEntry route : routeEntries) {
        fmt << "+--------------------------------------+" << '\n'
        << route.toString() << "\n"
        << "+--------------------------------------+" << '\n';
    }
    fmt << std::endl;
    std::string s = fmt.str();
    std::cout << s << std::endl;
    std::cout << "here" << std::endl;
    return s;
}