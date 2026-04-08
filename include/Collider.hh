#ifndef Pythia
#define Pythia 1

#include <string>

void runPythia(int nEvents, std::string pythiaConfigFile, std::string outputFile);
void protonGun(int nEvents, double E_GeV, std::string outputFile);

#endif