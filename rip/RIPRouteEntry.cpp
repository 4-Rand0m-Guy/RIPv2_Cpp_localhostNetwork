#include <ctime>
#include <sstream>
#include <iostream>
#include "RIPRouteEntry.h"

RIPRouteEntry::RIPRouteEntry(unsigned char *data) {
    deserialize(data);
//    init_timer();
}

RIPRouteEntry::RIPRouteEntry(unsigned _address, unsigned _nextHop, unsigned _metric) {
    afi = 0;
    tag = 0;
    address = _address;
    subnetMask = 0;
    nextHop = _nextHop;
    metric = _metric;
//    init_timer();
}

RIPRouteEntry::RIPRouteEntry(short FFFF, short _authenticationType, char *password) {
    afi = FFFF;
    authenticationType = _authenticationType;
    authentication = password;
}

void RIPRouteEntry::deserialize(unsigned char* outBuffer) {
    afi = outBuffer[0] << 8 | outBuffer[1];
    tag = outBuffer[2] << 8 | outBuffer[3];
}

void RIPRouteEntry::serialize(unsigned char* inBuffer) {

}

void RIPRouteEntry::init_timer() {
    time_t time = std::time(0);
}

std::string RIPRouteEntry::toString() {
    init_timer();
    char* time_s = ctime(&time);
    std::stringstream fmt;
    if (afi == 65535) { // "0xFFFF (this indicates authentication entry"
        fmt << "timer: " << time_s
            << "|  AFI: " << afi
            << "  | authtype: " << authenticationType
            << " |\n|       authentication       "
            << "|\n|            " << authentication << "            |";
    } else {
        fmt << "|   AFI: " << afi
            << "  | routetag: " << tag
            << " |\n|       IP Address: " << address
            << "     |\n|       subnet Mask: " << subnetMask
            << "    |\n|       next hop: " << nextHop
            << "       |\n|       metric: " << metric
            << "         |";
    }
    std::string s = fmt.str();
    return s;
}