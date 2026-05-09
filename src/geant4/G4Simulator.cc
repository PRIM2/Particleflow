// G4Simulator.cc
#include "G4Simulator.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4VisExecutive.hh"

#include <iostream>


G4Simulator::G4Simulator(int nEvents, const std::string& hepmcFile, bool uiMode, double thickness)
    : m_nEvents(nEvents), m_hepmcFile(hepmcFile), m_uiMode(uiMode), m_thickness(thickness) {}


void G4Simulator::run(int argc, char** argv) {
    
    // TODO: ENVIAR A UN LUGAR DECENTE PARA DEBUGGEAR A POSTERIOR, poner declaracion fuera y uso en condicional
    // Silence cout if not in UI mode, to avoid cluttering the output with Geant4 messages
    std::streambuf* old_cout = nullptr;
    if (!m_uiMode) {
        // Its possible to write messajes by cerr
        //     std::cerr << "This output works perfectly" << std::endl;
        old_cout =  std::cout.rdbuf();
        std::cout.setstate(std::ios_base::failbit);
    }


    auto* runManager = new G4RunManager();
    auto* detector = new DetectorConstruction();
    detector->wallThickness = m_thickness; // Le pasamos el valor al detector TODO: revisar
    runManager->SetUserInitialization(detector);


    G4PhysListFactory factory;
    G4VModularPhysicsList* physics = factory.GetReferencePhysList("FTFP_BERT");
    runManager->SetUserInitialization(physics);

    runManager->SetUserInitialization(new ActionInitialization(m_hepmcFile));

    auto* UImanager = G4UImanager::GetUIpointer();

    // TODO: Corregir events, en ui no quedan yo pondria que si hay -i no se hagan beams, solo manualmente o algo asi.
    UImanager->ApplyCommand("/run/initialize");
    if (!m_uiMode) {
        UImanager->ApplyCommand("/run/beamOn " + std::to_string(m_nEvents));
    }

    if (m_uiMode) {
        auto* visManager = new G4VisExecutive();
        visManager->Initialize();
        auto* ui = new G4UIExecutive(argc, argv);
        ui->SessionStart();
        delete ui;
        delete visManager;
    }
    delete runManager;

    // Restore cout
    if (!m_uiMode) {
        std::cout.clear();
        std::cout.rdbuf(old_cout);
    }

}


G4Simulator::~G4Simulator() {}