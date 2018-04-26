#include <sstream>
#include <cstring>
#include <iostream>
#include "RIPHeader.h"
#include "istream"

RIPHeader::RIPHeader(unsigned routerID) {
    init_from_host(routerID);
}

RIPHeader::RIPHeader(unsigned char* data) {
    init_from_net(data);
}

void RIPHeader::init_from_host(short id) {
    command = '2'; // Only type 2 is supported.
    version = '2'; // RIP Version v2
    routerID = id;
}

void RIPHeader::init_from_net(unsigned char* data) {
    deserialize(data);
}

void RIPHeader::deserialize(unsigned char* buffer){
    command = buffer[0];
    version = buffer[1];
    short sh = buffer[2] << 8 | buffer[3]; // convert last two bytes
    routerID = sh;                         // into short int
}

void RIPHeader::serialize(unsigned char* buffer1){
    unsigned char buffer[4];
    buffer[0] = command;
    buffer[1] = version;
    buffer[2] = (routerID >> 8) & 0xff; // convert short int to char
    buffer[3] = routerID & 0xff;        // last 2 bytes in char array
    for (int i = 0; i < 4; ++i) {       // represents the short int
        buffer1[i] = buffer[i];
    }
}

std::string RIPHeader::toString() {
    std::stringstream fmt;
    fmt << "| command " << command
        << " | version " << version
        << " | routerID " << routerID << "      | ";
    std::string  s = fmt.str();
    return s;
}