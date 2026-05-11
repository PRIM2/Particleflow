// pythiaCollider.cc
#include "generators/Generators.hh"
#include "geant4/G4Simulator.hh"
#include "config/ConfigParser.hh"

#include <unistd.h>
#include <iostream>
#include <string>
#include <exception>

int main(int argc, char** argv) {
    std::string configFile = std::string(PROJECT_DIR) + "/config/runs/first.toml";

    int opt;
    while ((opt = getopt(argc, argv, "hc:")) != -1) {
        switch (opt) {
            case 'c':
                configFile = std::string(PROJECT_DIR) + "/" + std::string(optarg);
                break;

            case 'h':
                std::cout
                    << "Usage:\n"
                    << "  ./pythiaCollider [-c configFile.toml] [-h]\n\n"
                    << "Options:\n"
                    << "  -c FILE   Run configuration file\n"
                    << "  -h        Show this help message\n\n"
                    << "TOML configuration:\n"
                    << "  The run configuration file defines [Run], [Pythia], [Pgun] and [Geant4].\n"
                    << "  Example file: config/runs/first.toml\n"
                    << "  Documentation: see README.md or notes/PYTHIA.md for the .toml format.\n";
                return 0;

            default:
                std::cerr << "Unknown option\n";
                return 1;
        }
    }

    config::SimulationConfig cfg;
    try {
        cfg = config::loadConfig(configFile);
    } catch (const std::exception& e) {
        std::cerr << "Config error: " << e.what() << "\n";
        return 1;
    }

    cfg.pythia.cmndFile = std::string(PROJECT_DIR) + "/" + cfg.pythia.cmndFile;
    cfg.run.hepmcOutput = std::string(PROJECT_DIR) + "/" + cfg.run.hepmcOutput;
    cfg.geant4.input    = std::string(PROJECT_DIR) + "/" + cfg.geant4.input;

    try {
        const bool usesPythia =
            cfg.run.mode == "pythia-and-run" ||
            cfg.run.mode == "pythia-only";

        const bool usesPgun =
            cfg.run.mode == "pgun-and-run" ||
            cfg.run.mode == "pgun-only";

        const bool shouldRunGeant4 =
            cfg.run.mode == "pythia-and-run" ||
            cfg.run.mode == "pgun-and-run" ||
            cfg.run.mode == "run-from-hepmc";

        if (!usesPythia && !usesPgun && !shouldRunGeant4) {
            std::cerr << "Invalid Run.mode: " << cfg.run.mode << "\n";
            std::cerr << "Allowed values:\n";
            std::cerr << "  - pythia-and-run\n";
            std::cerr << "  - pgun-and-run\n";
            std::cerr << "  - run-from-hepmc\n";
            std::cerr << "  - pgun-only\n";
            std::cerr << "  - pythia-only\n";
            return 1;
        }

        if (usesPythia) {
            runPythia(
                cfg.run.nEvents,
                cfg.pythia,
                cfg.run.hepmcOutput
            );
        }

        if (usesPgun) {
            particleGun(
                cfg.run.nEvents,
                cfg.pgun,
                cfg.run.hepmcOutput
            );
        }

        if (shouldRunGeant4) {
            G4Simulator sim(
                cfg.run.nEvents,
                cfg.geant4,
                cfg.geant4.input
            );

            sim.run(argc, argv);
        }

    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}