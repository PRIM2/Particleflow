// KillBoxSD.cc
#include "geant4/detectorSDs/KillBoxSD.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"

KillBoxSD::KillBoxSD(const G4String& name)
  : BaseSD(name)
{
}

G4bool KillBoxSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  if (!step) {
    return false;
  }

  auto pre = step->GetPreStepPoint();
  if (!pre) {
    return false;
  }

  // Solo actuar cuando la partícula entra en el volumen sensible.
  if (pre->GetStepStatus() != fGeomBoundary) {
    return false;
  }

  auto track = step->GetTrack();
  if (!track) {
    return false;
  }

  auto volume = pre->GetTouchableHandle()->GetVolume();
  if (!volume) {
    return false;
  }

  auto volumeName = volume->GetName();

  // Caso especial: cara -X de la KillBox.
  // Solo mata partículas que entran por esa cara viniendo hacia -X.
  if (volumeName == "KillBox_MX") {
    auto post = step->GetPostStepPoint();

    if (
      post &&
      post->GetStepStatus() == fGeomBoundary &&
      pre->GetMomentumDirection().x() <= 0
    ) {
      // auto info = ExtractStepInfo(step);
      // PrintStepInfo(info);

      track->SetTrackStatus(fStopAndKill);
      return true;
    }

    return false;
  }

  // // Resto de caras de la KillBox: mata siempre al entrar.
  // auto info = ExtractStepInfo(step);
  // PrintStepInfo(info);

  track->SetTrackStatus(fStopAndKill);

  return true;
}