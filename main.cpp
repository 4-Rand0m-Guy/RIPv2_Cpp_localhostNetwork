#include <string>
#include <iostream>
#include "config/ConfigImporter.h"
#include "config/ConsoleLogger.h"
#include "rip/Rip.h"
#include "rip/RIPPacket.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        ConsoleLogger::error(0, "Args");
    }

    if (argc > 2) {
        ConsoleLogger::warning(0, "Args");
    }

    const std::string filename = argv[1];
    ConfigImporter configImporter = ConfigImporter(filename);
    Configuration cfg = configImporter.get_configuration();
    Rip daemon = Rip(cfg.routerID, cfg.input_ports, cfg.outputs, cfg.timer);
    daemon.run();
    std::cout << "Crashing..." << std::endl;

   unsigned char arr[24] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                             '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                             '0', '1', '2', '3'};

//    unsigned char ha[4];
//    RIPHeader hdr = RIPHeader(420);
//    hdr.serialize(ha);
//    RIPPacket packet = RIPPacket(ha, 4);
//    unsigned char pa[4];
//    packet.serialize(pa);
//    RIPRouteEntry rte = RIPRouteEntry(9, 4, 8);
//    unsigned char ra[20];
//    rte.serialize(ra);
//    packet.addRoute(ra);
//
//    RIPPacket packet5 = RIPPacket(pa, 4);

//    std::cout << rte.toString() << std::endl;
//    std::string outp = packet5.toString();
//    std::cout << outp << std::endl;
//    std::cout << hdr.toString() << std::endl;
//    daemon.processResponse(ha);
    return 0;
}