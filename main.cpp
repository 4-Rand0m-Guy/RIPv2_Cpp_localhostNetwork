#include <string>
#include "ConfigImporter.h"
#include "ConsoleLogger.h"
#include "Rip.h"

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
    Rip(cfg.routerID, cfg.input_ports, cfg.outputs, cfg.timer);
    return 0;
}