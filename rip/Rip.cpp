#include <iostream>
#include "Rip.h"
#include "RIPHeader.h"

Rip::Rip(unsigned _routerID, std::vector<unsigned> _input_ports, std::vector<OutputInterface> _outputs, unsigned _timer /* = 30 */) {
    unsigned routerID = _routerID;
    std::vector<unsigned> input_ports = _input_ports;
    std::vector<OutputInterface> outputs = _outputs;
    unsigned timer = _timer;
}