// pythia_sim.cc
#include "Collider.hh"
#include "G4Simulator.hh"
#include <unistd.h>
#include <iostream>
#include <string>

int main(int argc, char** argv) {

    int nEvents = 100;
    std::string cmndFile     = std::string(PROJECT_DIR) + "/cmndPythia/test.cmnd";
    std::string hepmcFile    = std::string(BUILD_DIR)   + "/results/test_PYTHIA.hepmc";
    bool uiMode = false;
    double thickness = 0.5;
    double beamEnergy = 7000.0;
    int opt;
    while ((opt = getopt(argc, argv, "hp:n:t:E:o:i")) != -1) {
        switch (opt) {
            case 'n': nEvents  = std::stoi(optarg);           break;
            case 'p': cmndFile  = std::string(PROJECT_DIR) + std::string(optarg);                     break;
            case 't': thickness = std::stod(optarg);          break;
            case 'E': beamEnergy = std::stod(optarg);         break;
            case 'i': uiMode    = true;                       break;
            case 'o': hepmcFile = std::string(PROJECT_DIR) + std::string(optarg);                break;
            case 'h':
                std::cout << "Usage: ./pythiaCollider [-n nEvents] [-p pythia.cmnd] [-t thickness (m)] [-E beamEnergy (GeV)] [-o filesName] [-i]\n";
                return 0;
            default:
                std::cerr << "Unknown option\n";
                return 1;
        }
    }

    runPythia(nEvents, cmndFile, hepmcFile, beamEnergy);
    
    G4Simulator sim(nEvents, hepmcFile, uiMode, thickness);
    sim.run(argc, argv);

    return 0;
}