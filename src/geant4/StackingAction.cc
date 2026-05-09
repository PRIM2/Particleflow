// StackingAction.cc
#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

#include <cmath>

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track)
{
    // Parameters (update from somewhere :( )
    G4ThreeVector detectorCenter(
        29.74142*m,
        -4.26804*m,
        9.33062*m
    );

    G4ThreeVector detectorTargetOffset(20*cm, 0., 0.);
    G4double maxAcceptanceAngle = 50.0*deg;

    // Particle state
    G4ThreeVector particlePos = track->GetPosition();
    G4ThreeVector particleDir = track->GetMomentumDirection();

    // Direction from particle to detector target point
    G4ThreeVector detectorTarget = detectorCenter + detectorTargetOffset;
    G4ThreeVector toDetector = detectorTarget - particlePos;

    if (toDetector.mag2() == 0.) {
        return fUrgent;
    }

    G4ThreeVector dirToDetector = toDetector.unit();

    // Angular acceptance
    //
    // cosTheta = particleDir · dirToDetector
    //
    // cosTheta = 1  -> particle points exactly to the detector target
    // cosTheta = 0  -> particle moves perpendicular to the detector direction
    // cosTheta < 0  -> particle moves away from the detector
    //
    // Keep only particles inside a cone of aperture maxAcceptanceAngle
    // around the direction pointing to the detector.
    G4double cosTheta = particleDir.dot(dirToDetector);

    if (cosTheta >= std::cos(maxAcceptanceAngle)) {
        return fUrgent;
    }

    return fKill;
}