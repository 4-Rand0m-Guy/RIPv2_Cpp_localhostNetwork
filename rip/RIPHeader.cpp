#include <sstream>
#include <cstring>
#include <iostream>
#include "RIPHeader.h"
#include "istream"

RIPHeader::RIPHeader(unsigned short _routerID) {
    command = '2'; // Only type 2 is supported.
    version = '2'; // RIP Version v2
    routerID = _routerID;
}

RIPHeader::RIPHeader(unsigned char* data) {
    deserialize(data);
}

void RIPHeader::deserialize(unsigned char* buffer){
    command = buffer[0];
    version = buffer[1];
    routerID = buffer[2] << 8 | buffer[3];
}

void RIPHeader::serialize(unsigned char* inBuffer){
    // last 2 bytes in char array represents the unsigned short
    unsigned char buffer[4] = {command,version,(routerID >> 8) & 0xff, routerID & 0xff};
    for (int i = 0; i < 4; ++i) {
        inBuffer[i] = buffer[i];
    }
}

std::string RIPHeader::toString() {
    std::stringstream fmt;
    fmt << "| command " << command
        << " | version " << version
        << " | routerID " << routerID << " | ";
    std::string s = fmt.str();
    return s;
}