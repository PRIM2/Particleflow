#pragma once

#include "G4VSensitiveDetector.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "HepMC3/WriterAscii.h"

#include <memory>
#include <string>
#include <vector>

class G4Step;
class G4TouchableHistory;
class G4HCofThisEvent;

struct StepRecord {
  G4double t0 = 0.0;
  G4ThreeVector p0;
  G4ThreeVector p1;
  G4double edep = 0.0;
  G4int pdg = 0;
  G4int trackID = -1;
  G4ThreeVector momentum;
  G4double kineticE = 0.0;
  G4double totalE = 0.0;
};

class BaseSD : public G4VSensitiveDetector {
public:
  explicit BaseSD(const G4String& name);
  ~BaseSD() override;

  static void SetOutputFile(const std::string& outputFile);

  void Initialize(G4HCofThisEvent* hce) override;
  void EndOfEvent(G4HCofThisEvent* hce) override;

protected:
  void RecordStep(G4Step* step);

  void ExportHepMC3(
    G4int globalEventID,
    const std::vector<StepRecord>& steps,
    const std::string& hepmcFilename
  );

  static std::string sOutputFile;

  std::vector<StepRecord> fSteps;
  std::unique_ptr<HepMC3::WriterAscii> fWriterHepMC3;
  G4int fTotalEventsProcessed = 0;
};