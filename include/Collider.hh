#ifndef Pythia
#define Pythia 1

#include <string>

void runPythia(
    int nEvents, 
    std::string pythiaConfigFile, 
    std::string outputFile, 
    double beamEnergy
);

void particleGun(
    int nEvents,
    double E_GeV,
    std::string outputFile,
    int pdg = 2212,
    double mass_GeV = 0.938272
);

#endif