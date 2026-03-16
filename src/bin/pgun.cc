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

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "Messages.hh"
#include "Pythia8/Pythia.h"


int main(int argc, char** argv) {
  // Header
  G4cout << "\nGeant4 version: " << G4Version << G4endl;	
  G4cout << msg::OUT << " pgun.cc working\n" << G4endl;
  //G4cerr << msg::ERR_INI <<"This is an error" << msg::ERR_END << G4endl;



  /* -------- CODE  --------- */
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

  // Si pasas un macro como argumento: ./pgun macros/init.mac
  if (argc > 1) {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  } else {
    // Modo interactivo
    auto* ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute macros/init.mac");
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;


  /* --------- Code Ending --------- */
  G4cout << "\n\nNormal Termination at " << msg::ExecutionTime() << G4endl;

  return 0;
}
