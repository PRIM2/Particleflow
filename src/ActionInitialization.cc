#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"

void ActionInitialization::Build() const {
  // PArticle gun
  //SetUserAction(new PrimaryGeneratorAction());

  SetUserAction(new PrimaryGeneratorAction(std::string(BUILD_DIR) + "/tmp/" "test.hepmc"));
}

void ActionInitialization::BuildForMaster() const {
  // Vacío de momento (útil en MT para RunAction)
}
