#include "geant4/detectorSDs/RPCSD.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"

RPCSD::RPCSD(const G4String& name): BaseSD(name) {}

G4bool RPCSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
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

  auto track = step->GetTrack();

  if (!track) {
    return false;
  }

  RecordStep(step);

  track->SetTrackStatus(fStopAndKill);

  return true;
}