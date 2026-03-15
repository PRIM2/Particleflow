#include "GasSD.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHandle.hh"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem> // to create /build/results directory

GasSD::GasSD(const G4String& name)
  : G4VSensitiveDetector(name)
{}

// Se llama al principio de cada evento
void GasSD::Initialize(G4HCofThisEvent* /*hce*/)
{
  fEdepTot = 0.;
  fNsteps  = 0;
  fTrackIDs.clear();
  fSteps.clear();
}

G4bool GasSD::ProcessHits(G4Step* step, G4TouchableHistory* /*history*/)
{
  const auto edep = step->GetTotalEnergyDeposit();
  if (edep <= 0.) return false;

  auto track = step->GetTrack();
  auto pre   = step->GetPreStepPoint();
  auto post  = step->GetPostStepPoint();

  // Datos que pides
  const G4double t0 = pre->GetGlobalTime();
  const G4ThreeVector p0 = pre->GetPosition();
  const G4ThreeVector p1 = post->GetPosition();

  const G4int pdg     = track->GetDefinition()->GetPDGEncoding();
  const G4int trackID = track->GetTrackID();

  // volume_id: nombre + (opcional) copyNo
  auto vol = pre->GetTouchableHandle()->GetVolume();
  const G4String volName = vol ? vol->GetName() : "UNKNOWN";
  const G4int    copyNo  = vol ? vol->GetCopyNo() : -1;

  StepRecord rec;
  rec.t0 = t0;
  rec.p0 = p0;
  rec.p1 = p1;
  rec.edep = edep;
  rec.trackID = trackID;
  rec.pdg = pdg;
  rec.volumeName = volName;
  rec.volumeCopyNo = copyNo;

  fSteps.push_back(rec);

  // Acumuladores (siguen siendo útiles)
  fEdepTot += edep;
  ++fNsteps;
  fTrackIDs.insert(trackID);

  return true;
}

void GasSD::EndOfEvent(G4HCofThisEvent* /*hce*/)
{
  auto rm = G4RunManager::GetRunManager();
  auto ev = rm->GetCurrentEvent();
  if (!ev) return;

  const G4int eventID = ev->GetEventID();
  const G4int nTracks = static_cast<G4int>(fTrackIDs.size());

  // DATA LOG INSTO /build/results/...
  std::filesystem::create_directories(std::string(BUILD_DIR) + "/results");
  
  std::ostringstream fname;  
  fname << BUILD_DIR << "/results/gas_event_" << eventID << ".dat";

  std::ofstream out(fname.str());
  if (!out.is_open()) return;

  // (Opcional) resumen por evento: lo mantengo porque ayuda a validar
  out << "# eventID  Edep_gas_total[MeV]  Nsteps_in_gas  Ntracks_in_gas\n";
  out << eventID << " "
      << std::setprecision(10) << (fEdepTot/MeV) << " "
      << fNsteps << " "
      << nTracks << "\n\n";

  // CSV pedido (con unidades explícitas en cabecera)
  out << "t0[ns],x0[mm],y0[mm],z0[mm],x1[mm],y1[mm],z1[mm],Edep[MeV],event_id,track_id,pdg,volume_name,volume_copyNo\n";

  out << std::setprecision(10);
  for (const auto& s : fSteps) {
    out << (s.t0/ns) << ","
        << (s.p0.x()/mm) << "," << (s.p0.y()/mm) << "," << (s.p0.z()/mm) << ","
        << (s.p1.x()/mm) << "," << (s.p1.y()/mm) << "," << (s.p1.z()/mm) << ","
        << (s.edep/MeV) << ","
        << eventID << ","
        << s.trackID << ","
        << s.pdg << ","
        << s.volumeName << ","
        << s.volumeCopyNo
        << "\n";
  }

  out.close();
}
