#pragma once

#include "geant4/detectorSDs/BaseSD.hh"

#include "G4String.hh"
#include "globals.hh"

class G4Step;
class G4TouchableHistory;

class KillBoxSD : public BaseSD {
public:
  explicit KillBoxSD(const G4String& name);
  ~KillBoxSD() override = default;

  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
};