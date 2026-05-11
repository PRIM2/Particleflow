// G4Simulator.hh
#ifndef G4SIMULATOR_HH
#define G4SIMULATOR_HH

#include "config/ConfigParser.hh"

#include <string>

class G4Simulator {
public:
    G4Simulator(
        int nEvents,
        const config::Geant4Config& cfg,
        const std::string& hepmcFile
    );

    ~G4Simulator();

    void run(int argc, char** argv);

private:
    int m_nEvents;
    std::string m_hepmcFile;
    config::Geant4Config m_cfg;
};

#endif