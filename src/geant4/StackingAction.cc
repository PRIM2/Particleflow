#include "geant4/StackingAction.hh"

#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

#include <cmath>

StackingAction::StackingAction(const config::Geant4Config& cfg)
    : m_cfg(cfg)
{
    const auto& filter = m_cfg.angularFilter;

}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track)
{
    const auto& filter = m_cfg.angularFilter;

    if (!filter.enabled) {
        return fUrgent;
    }

    const G4ThreeVector filterTarget(
        filter.targetX * m,
        filter.targetY * m,
        filter.targetZ * m
    );

    const G4double maxAcceptanceAngle = filter.theta * deg;
    const G4double cosMaxAcceptanceAngle = std::cos(maxAcceptanceAngle);

    const G4ThreeVector particlePos = track->GetPosition();
    const G4ThreeVector particleDir = track->GetMomentumDirection();

    const G4ThreeVector toFilterTarget = filterTarget - particlePos;

    if (toFilterTarget.mag2() == 0.) {
        return fUrgent;
    }

    const G4ThreeVector dirToFilterTarget = toFilterTarget.unit();

    const G4double cosTheta = particleDir.dot(dirToFilterTarget);

    if (cosTheta >= cosMaxAcceptanceAngle) {
        return fUrgent;
    }

    return fKill;
}

