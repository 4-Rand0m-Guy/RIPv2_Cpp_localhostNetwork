#include "RIPPacket.h"

RIPPacket::RIPPacket(unsigned char *data) {
    deserialize(data);
}

RIPPacket::RIPPacket(RIPHeader header1) {
    header = &header1;
}

void RIPPacket::deserialize(unsigned char *outBuffer) {
    unsigned char hdr[4] = {outBuffer[0],outBuffer[1],outBuffer[2],outBuffer[3]};
    *header = RIPHeader(hdr);

//    for (auto i = 4; i < 504; i +=4) {
//
//    }
}