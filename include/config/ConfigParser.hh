// ConfigParser.hh
#pragma once

#include <string>

namespace config {

    struct RunConfig {
        int nEvents = 100;
        std::string mode = "pythia-and-run";
        std::string hepmcOutput = "results/test_GENERATED.hepmc";
    };

    struct PythiaConfig {
        std::string cmndFile = "config/cmndPythia/test.cmnd";
    };

    struct PgunConfig {
        double energy = 100.0;
        int pdg = 2212;
        double mass = 0.938272;
    };

    struct AngularFilterConfig {
        bool enabled = false;

        // Target point for the angular filter.
        // Units assumed by the parser/user config: metres.
        double targetX = 0.0;
        double targetY = 0.0;
        double targetZ = 0.0;

        // Maximum acceptance angle.
        // Units assumed by the parser/user config: degrees.
        double theta = 50.0;
    };

    struct Geant4Config {
        std::string input = "results/test_GENERATED.hepmc";
        std::string output = "results/test_PARTICLEFLOW.hepmc";
        bool ui = false;
        std::string geometry = "SimpleWall";

        AngularFilterConfig angularFilter;
    };

    struct SimulationConfig {
        RunConfig run;
        PythiaConfig pythia;
        PgunConfig pgun;
        Geant4Config geant4;
    };

    SimulationConfig loadConfig(const std::string& filename);

} // namespace config