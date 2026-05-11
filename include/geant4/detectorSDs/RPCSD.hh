#pragma once

#include "geant4/detectorSDs/BaseSD.hh"

#include "G4String.hh"
#include "globals.hh"

class G4Step;
class G4TouchableHistory;

class RPCSD : public BaseSD {
public:
  explicit RPCSD(const G4String& name);
  ~RPCSD() override = default;

  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
};