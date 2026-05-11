// StackingAction.hh
#pragma once

#include "G4UserStackingAction.hh"
#include "G4ClassificationOfNewTrack.hh"
#include "G4ThreeVector.hh"

#include "config/ConfigParser.hh"

class StackingAction : public G4UserStackingAction
{
public:
    explicit StackingAction(const config::Geant4Config& cfg);

    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* track) override;

private:
    const config::Geant4Config& m_cfg;
};