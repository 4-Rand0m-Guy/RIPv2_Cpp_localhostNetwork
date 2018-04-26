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
    std::cout << "Input ports for daemon = " << std::endl;
    for (unsigned sock: daemon.getInput_ports()) {
        std::cout << sock << std::endl;
    }
    daemon.run();
    std::cout << "Daemon started" << std::endl;
    return 0;
}