// pythia_sim.cc
#include "Collider.hh"
#include "G4Simulator.hh"
#include <unistd.h>
#include <iostream>
#include <string>

int main(int argc, char** argv) {

    int nEvents = 100;
    std::string cmndFile     = std::string(PROJECT_DIR) + "/src/test.cmnd";
    std::string hepmcFile    = std::string(BUILD_DIR)   + "/results/test_PYTHIA.hepmc";
    bool uiMode = false;

    int opt;
    while ((opt = getopt(argc, argv, "hp:n:i")) != -1) {
        switch (opt) {
            case 'n': nEvents  = std::stoi(optarg);           break;
            case 'p': cmndFile  = optarg;                     break;
            case 'i': uiMode    = true;                       break;
            case 'h':
                std::cout << "Usage: ./pythia_sim [-n nEvents] [-p pythia.cmnd] [-i]\n";
                return 0;
            default:
                std::cerr << "Unknown option\n";
                return 1;
        }
    }

    runPythia(nEvents, cmndFile, hepmcFile);

    G4Simulator sim(nEvents, hepmcFile, uiMode);
    sim.run(argc, argv);

    return 0;
}