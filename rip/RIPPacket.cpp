#include <sstream>
#include <iostream>
#include <fstream>
#include "RIPPacket.h"

RIPPacket::RIPPacket(unsigned char* data, int length) {
    message = data;
    cur_len = length;
    deserialize(data, length);
}

RIPPacket::RIPPacket(RIPHeader* _header) {
    header = _header;
    /*cur_len = 4;
    _header->serialize(message);*/
}



void RIPPacket::deserialize(unsigned char *outBuffer, int length) {
    /*unsigned char hdr_arr[4] = {outBuffer[0],outBuffer[1],outBuffer[2],outBuffer[3]};
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
    }*/
}

void RIPPacket::serialize(std::ostream &stream) {
    stream << header->getCommand() << header->getVersion() << header->getRouterID();
    for (RIPRouteEntry entry: routeEntries) {
        stream << entry.getAfi() << entry.getTag() << entry.getAddress() << entry.getSubnetMask() << entry.getNextHop
                () << entry.getAuthentication() << entry.getMetric() << entry.getTime();
    }

}

void RIPPacket::addRoute(RIPRouteEntry rte) {
    routeEntries.push_back(rte);
}


RIPHeader RIPPacket::getHeader() {
    return *header;
}

std::vector<RIPRouteEntry> RIPPacket::routes() {
    return routeEntries;
}

std::string RIPPacket::toString() {
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
    return s;
}

