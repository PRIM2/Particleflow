#ifndef GasSD_h
#define GasSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <vector>
#include <set>
#include <string>

class G4Step;
class G4TouchableHistory;
class G4HCofThisEvent;

class GasSD : public G4VSensitiveDetector {
public:
  GasSD(const G4String& name);
  ~GasSD() override = default;

  void Initialize(G4HCofThisEvent* hce) override;
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
  void EndOfEvent(G4HCofThisEvent* hce) override;

private:
  struct StepRecord {
    G4double t0;
    G4ThreeVector p0;
    G4ThreeVector p1;
    G4double edep;
    G4int trackID;
    G4int pdg;
    G4String volumeName;   // "volume_id" como nombre
    G4int volumeCopyNo;    // opcional: por si quieres un id entero
  };

  std::vector<StepRecord> fSteps;

  // Mantengo tus acumuladores por evento (por si aún los quieres)
  G4double        fEdepTot = 0.;
  G4int           fNsteps  = 0;
  std::set<G4int> fTrackIDs;
};

#endif
