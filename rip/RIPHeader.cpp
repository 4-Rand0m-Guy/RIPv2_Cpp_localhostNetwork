#include <sstream>
#include <cstring>
#include <iostream>
#include "RIPHeader.h"
#include "istream"

RIPHeader::RIPHeader(unsigned short _routerID) {
    command = '2'; // Only command 2 (Response) is supported.
    version = '2'; // RIP Version 2
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

/*void RIPHeader::serialize(unsigned char* inBuffer){
    // last 2 bytes in char array represents the unsigned short
    unsigned char buffer[4] = {command,version,(routerID >> 8) & 0xff, routerID & 0xff};
    for (int i = 0; i < 4; ++i) {
        inBuffer[i] = buffer[i];
    }
    delete buffer;
}*/

unsigned char * RIPHeader::serialize2() {
    auto * buffer = new unsigned char[4];
    buffer[0] = command;
    buffer[1] = version;
    buffer[2] = static_cast<unsigned char>((routerID >> 8) & 0xff);
    buffer[3] = static_cast<unsigned char>(routerID & 0xff);
    return buffer;
}

std::string RIPHeader::toString() {
    std::stringstream fmt;
    fmt << "| command " << command
        << " | version " << version
        << " | routerID " << routerID << " | " << std::endl;
    std::string s = fmt.str();
    return s;
}

unsigned char RIPHeader::getCommand() const {
    return command;
}

void RIPHeader::setCommand(unsigned char command) {
    RIPHeader::command = command;
}

unsigned char RIPHeader::getVersion() const {
    return version;
}

void RIPHeader::setVersion(unsigned char version) {
    RIPHeader::version = version;
}

unsigned short RIPHeader::getRouterID() const {
    return routerID;
}

void RIPHeader::setRouterID(unsigned short routerID) {
    RIPHeader::routerID = routerID;
}
