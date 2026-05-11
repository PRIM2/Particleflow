#pragma once

#include "geant4/detectorSDs/BaseSD.hh"

#include "G4String.hh"
#include "globals.hh"

class G4Step;
class G4TouchableHistory;

class ScintillatorSD : public BaseSD {
public:
  explicit ScintillatorSD(const G4String& name);
  ~ScintillatorSD() override = default;

  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
};