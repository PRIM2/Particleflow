// pgun.cc
#include "G4Version.hh"
#include "G4ios.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"

#include <ios>
#include <unistd.h>   // getopt


#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "Pythia8/Pythia.h"


#include "Collider.hh"

int main(int argc, char** argv) {

    int opt;
    int nEvents = 100;  // Valor por defecto
    G4String geantConfigFile = "run.mac"; 
    std::string pythiaConfigFile = "test.cmnd";
    bool uiMode = false;


    while ((opt = getopt(argc, argv, "hc:p:n:i")) != -1) {
        switch (opt) {
            case 'h': {
                std::cout << "Usage: ./pgun\n";
                std::cout << "  -c <config.macro>   Geant4 macro file\n";
                std::cout << "  -p <pythia.cmnd>    Pythia config file\n";
                std::cout << "  -n <events>         Number of pythia events\n";
                std::cout << "  -i                  Interactive mode\n";
                return 0;
            }

            case 'c': {
                geantConfigFile = optarg;
                break;
            }

            case 'p': {
                pythiaConfigFile = optarg;
                break;
            }

            case 'n': {
                nEvents = std::stoi(optarg);
                break;
            }

            case 'i': {
                uiMode = true;
                break;
            }

            default: {
                std::cerr << "Unknown option\n";
                return 1;
            }
        }
    }

  
  /* -------- CODE  --------- */
  /*
  
 // 1) Run manager (secuencial)
  auto* runManager = new G4RunManager();

  // 2) Geometría
  runManager->SetUserInitialization(new DetectorConstruction());

  // 3) Física (lista de referencia)
  G4PhysListFactory factory;
  G4VModularPhysicsList* physics = factory.GetReferencePhysList("FTFP_BERT");
  runManager->SetUserInitialization(physics);

  // 4) Acciones (aquí registras el gun en Build())
  runManager->SetUserInitialization(new ActionInitialization());

  // 5) Visualización (opcional, pero útil)
  auto* visManager = new G4VisExecutive();
  visManager->Initialize();

  // 6) UI manager
  auto* UImanager = G4UImanager::GetUIpointer();

  
  G4String command = "/control/execute "; // command to execute macro files (always same)

  UImanager->ApplyCommand(command + std::string(PROJECT_DIR) + "/macros/" + geantConfigFile);

  if (uiMode) {
    // Modo interactivo
    auto* ui = new G4UIExecutive(argc, argv);
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;


  
  */

  pruebasPythia(nEvents, pythiaConfigFile);


  std::cout << "\n\nPYTHIA.CC IS WORKING: " << prueba() << std::endl;
  std::cout << "Pythia initialized with config file: " << PROJECT_DIR <<  "/src/test.cmnd" << std::endl;
  return 0;
  
}
