#include <string>
#include <iostream>
#include "config/ConfigImporter.h"
#include "config/ConsoleLogger.h"
#include "rip/Rip.h"

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
    std::cout << "THE DAEMON HAS CRASHED!!" << std::endl;
    return 0;
}