#include <ctime>
#include <sstream>
#include <iostream>
#include "RIPRouteEntry.h"

RIPRouteEntry::RIPRouteEntry(unsigned char *data) {
//    deserialize();
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

RIPRouteEntry::RIPRouteEntry(short FFFF, short _authenticationType, unsigned char* password) {
    afi = FFFF;
    authenticationType = _authenticationType;
    for (auto i = 0; i < 16; ++i) {
        authentication[i] = password[i];
    }
}

void RIPRouteEntry::deserialize(std::istream &stream) {
    stream >> afi >> tag >> address >> subnetMask >> nextHop >> metric;
    /*afi = outBuffer[0] << 8 | outBuffer[1];
    if (afi == 65535) {
        authenticationType = outBuffer[2] << 8 | outBuffer[3];
        for (auto i = 4; i < 16; ++i) {
            authentication[i-4] = outBuffer[i];
        }
    } else {
        tag = outBuffer[2] << 8 | outBuffer[3];
        address = ch2uint(outBuffer, 4);
        subnetMask = ch2uint(outBuffer, 8);
        nextHop = ch2uint(outBuffer, 12);
        metric = ch2uint(outBuffer, 16);
    }*/
}

void RIPRouteEntry::serialize(std::ostream &stream) {
    stream << afi << tag << address << subnetMask << nextHop << metric;
    /*unsigned char buffer[20];
    buffer[0] = (afi >> 8) & 0xff; buffer[1] = afi & 0xff;
    if (afi == 65535) {
        buffer[2] = (authenticationType >> 8) & 0xff; buffer[3] = authenticationType & 0xff;
        for (auto i = 4; i < 20; ++i) {
            buffer[i] = authentication[i-4];
        }
    } else {
        buffer[2] = (tag >> 8) & 0xff;
        buffer[3] = tag & 0xff;
        uint2ch(address, buffer, 4);
        uint2ch(subnetMask, buffer, 8);
        uint2ch(nextHop, buffer, 12);
        uint2ch(metric, buffer, 16);
    }
    for (auto i = 0; i < 20; ++i) {
        inBuffer[i] = buffer[i];
    }*/
}

void RIPRouteEntry::init_timer() {
    time = std::time(0);
}

std::string RIPRouteEntry::toString() {
    char* time_s = ctime(&time);
    std::stringstream fmt;
    if (afi == 65535) { // "0xFFFF (this indicates authentication entry"
        fmt << "timer: " << time_s
            << "|     AFI: " << afi
            << "     |    authtype: " << authenticationType
            << "     |\n|       authentication       "
            << "|\n|            " << authentication << "            |";
    } else {
        fmt << "|      AFI: " << afi
            << "     |    routetag:     " << tag
            << " |\n|             IP Address: " << address
            << "            |\n|             subnet Mask: " << subnetMask
            << "           |\n|             next hop: " << nextHop
            << "              |\n|             metric: " << metric
            << "                |";
    }
    std::string s = fmt.str();
    return s;
}

unsigned RIPRouteEntry::ch2uint(unsigned char* chars, int index) {
    typedef unsigned char u8;  // in case char is signed by default on your platform
    return ((u8)chars[index] << 24) | ((u8)chars[index+1] << 16) | ((u8)chars[index+2] << 8) | (u8)chars[index+3];
}

void RIPRouteEntry::uint2ch(unsigned ui, unsigned char* buf, int index) {
    buf[index] = (ui >> 24) & 0xFF;
    buf[index+1] = (ui >> 16) & 0xFF;
    buf[index+2] = (ui >> 8) & 0xFF;
    buf[index+3] = ui & 0xFF;
}

int RIPRouteEntry::getNextHop() const {
    return nextHop;
}

void RIPRouteEntry::setMetric(int metric) {
    RIPRouteEntry::metric = metric;
}

unsigned short RIPRouteEntry::getAfi() const {
    return afi;
}

void RIPRouteEntry::setAfi(unsigned short afi) {
    RIPRouteEntry::afi = afi;
}

unsigned short RIPRouteEntry::getTag() const {
    return tag;
}

void RIPRouteEntry::setTag(unsigned short tag) {
    RIPRouteEntry::tag = tag;
}

int RIPRouteEntry::getAddress() const {
    return address;
}

void RIPRouteEntry::setAddress(int address) {
    RIPRouteEntry::address = address;
}

int RIPRouteEntry::getSubnetMask() const {
    return subnetMask;
}

void RIPRouteEntry::setSubnetMask(int subnetMask) {
    RIPRouteEntry::subnetMask = subnetMask;
}

void RIPRouteEntry::setNextHop(int nextHop) {
    RIPRouteEntry::nextHop = nextHop;
}

short RIPRouteEntry::getAuthenticationType() const {
    return authenticationType;
}

void RIPRouteEntry::setAuthenticationType(short authenticationType) {
    RIPRouteEntry::authenticationType = authenticationType;
}

const unsigned char *RIPRouteEntry::getAuthentication() const {
    return authentication;
}

int RIPRouteEntry::getMetric() const {
    return metric;
}

time_t RIPRouteEntry::getTime() const {
    return time;
}

void RIPRouteEntry::setTime(time_t time) {
    RIPRouteEntry::time = time;
}
