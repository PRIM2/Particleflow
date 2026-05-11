// Generators.hh
#ifndef GENERATORS_HH
#define GENERATORS_HH

#include "config/ConfigParser.hh"

#include <string>

void runPythia(
    int nEvents,
    const config::PythiaConfig& cfg,
    const std::string& outputFile
);

void particleGun(
    int nEvents,
    const config::PgunConfig& cfg,
    const std::string& outputFile
);

#endif