// G4Simulator.cc
#include "geant4/G4Simulator.hh"

#include "geant4/DetectorConstruction.hh"
#include "geant4/ActionInitialization.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4VisExecutive.hh"

#include <iostream>


G4Simulator::G4Simulator(
    int nEvents,
    const config::Geant4Config& cfg,
    const std::string& hepmcFile
)
    : m_nEvents(nEvents),
      m_hepmcFile(hepmcFile),
      m_cfg(cfg) {}


void G4Simulator::run(int argc, char** argv) {
    std::streambuf* old_cout = nullptr;

    if (!m_cfg.ui) {
        old_cout = std::cout.rdbuf();
        std::cout.setstate(std::ios_base::failbit);
    }

    auto* runManager = new G4RunManager();

    auto* detector = new DetectorConstruction(m_cfg.geometry, m_cfg.output);
    runManager->SetUserInitialization(detector);

    G4PhysListFactory factory;
    G4VModularPhysicsList* physics = factory.GetReferencePhysList("FTFP_BERT");
    runManager->SetUserInitialization(physics);

    runManager->SetUserInitialization(new ActionInitialization(m_hepmcFile, m_cfg));

    auto* UImanager = G4UImanager::GetUIpointer();

    UImanager->ApplyCommand("/run/initialize");

    if (!m_cfg.ui) {
        UImanager->ApplyCommand("/run/beamOn " + std::to_string(m_nEvents));
    }

    if (m_cfg.ui) {
        auto* visManager = new G4VisExecutive();
        visManager->Initialize();

        auto* ui = new G4UIExecutive(argc, argv);
        ui->SessionStart();

        delete ui;
        delete visManager;
    }

    delete runManager;

    if (!m_cfg.ui) {
        std::cout.clear();
        std::cout.rdbuf(old_cout);
    }
}


G4Simulator::~G4Simulator() {}