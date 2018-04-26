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
    unsigned short sh = buffer[2] << 8 | buffer[3]; // convert last two bytes
    routerID = sh;                         // into unsigned shorts
}

void RIPHeader::serialize(unsigned char* buffer1){
    unsigned char buffer[4];
    buffer[0] = command;
    buffer[1] = version;
    buffer[2] = (routerID >> 8) & 0xff; // convert unsigned short to char
    buffer[3] = routerID & 0xff;        // last 2 bytes in char array
    for (int i = 0; i < 4; ++i) {       // represents the unsigned short
        buffer1[i] = buffer[i];
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