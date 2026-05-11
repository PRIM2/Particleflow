// GenericSD.cc
#include "geant4/detectorSDs/GenericSD.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"

GenericSD::GenericSD(const G4String& name)
  : BaseSD(name)
{
}

G4bool GenericSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  if (!step) {
    return false;
  }

  auto pre = step->GetPreStepPoint();
  if (!pre) {
    return false;
  }

  if (pre->GetStepStatus() != fGeomBoundary) {
    return false;
  }

  RecordStep(step);

  return true;
}