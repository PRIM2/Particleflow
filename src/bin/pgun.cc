#include "Collider.hh"
#include "G4Simulator.hh"
#include <unistd.h>
#include <iostream>


int main(int argc, char** argv) {
    // Predetermined parameters
    int nEvents = 100;
    std::string hepmcFile = std::string(BUILD_DIR)   + "/results/test_PYTHIA.hepmc";
    bool uiMode = false;
    double E_GeV = 7000.0;
    
    int opt;
    while ((opt = getopt(argc, argv, "hn:iE:")) != -1) {
        switch (opt) {
            case 'n': nEvents  = std::stoi(optarg); break;
            case 'i': uiMode   = true;               break;
            case 'E': E_GeV    = std::stod(optarg);  break;
            case 'h':
                std::cout << "Usage: ./pgun [-n nEvents] [-i] [-E energy]\n";
                return 0;
            default:
                std::cerr << "Unknown option\n";
                return 1;
        }
    }

    protonGun(nEvents, E_GeV, hepmcFile);

    G4Simulator sim(nEvents, hepmcFile, uiMode);
    sim.run(argc, argv);

    return 0;
}
