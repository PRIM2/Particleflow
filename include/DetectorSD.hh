#ifndef DetectorSD_h
#define DetectorSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "HepMC3/GenEvent.h"
#include "HepMC3/WriterAscii.h"

#include <vector>
#include <memory> // Para std::unique_ptr

class G4Step;
class G4TouchableHistory;
class G4HCofThisEvent;

class DetectorSD : public G4VSensitiveDetector {
public:
  DetectorSD(const G4String& name);
  
  // IMPORTANTE: El destructor ya no puede ser default, 
  // necesitamos que cierre el archivo al terminar el programa.
  virtual ~DetectorSD(); 

  void Initialize(G4HCofThisEvent* hce) override;
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
  void EndOfEvent(G4HCofThisEvent* hce) override;

private:
  struct StepRecord {
    G4double      t0, edep, kineticE, totalE; 
    G4ThreeVector p0, p1, momentum;
    G4int         pdg, trackID;
  };

  // Solo necesitamos una función de exportación
  void ExportHepMC3(G4int globalEventID, const std::vector<StepRecord>& steps);
  void ExportHepMC3(G4int globalEventID, const std::vector<StepRecord>& steps, const std::string& hepmcFilename);
  // Miembros de datos
  std::vector<StepRecord> fSteps;
  std::unique_ptr<HepMC3::WriterAscii> fWriterHepMC3;

  // ESTA ES LA CLAVE: El contador que nunca vuelve a cero
  G4int fTotalEventsProcessed; 

  // Acumuladores temporales por cada evento (se limpian en Initialize)
  G4double fEdepTot;
};

#endif