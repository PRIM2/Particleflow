#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

// HepMC3
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/GenEvent.h"

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  PrimaryGeneratorAction(const std::string& filename);
  ~PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* event) override;  // OBLIGATORIO

private:
  //G4ParticleGun* fGun = nullptr;
  HepMC3::ReaderAscii m_reader;
};

#endif
