// DetectorSD.cc
#include "DetectorSD.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHandle.hh"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/FourVector.h"

#include <filesystem>


DetectorSD::DetectorSD(const G4String& name): 
  G4VSensitiveDetector(name), fTotalEventsProcessed(0) {}


DetectorSD::~DetectorSD() 
{
  if (fWriterHepMC3) fWriterHepMC3->close();
}


void DetectorSD::Initialize(G4HCofThisEvent* /*hce*/)
{
  fSteps.clear();
}


// Called for each step inside the sensitive detector
G4bool DetectorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  auto pre = step->GetPreStepPoint();

  if (pre->GetStepStatus() != fGeomBoundary) return false;

  auto track = step->GetTrack();


  if (GetName() == "KillBox") {
    auto volumeName = pre->GetTouchableHandle()->GetVolume()->GetName();
    if (volumeName == "KillBox_MX") {
      auto post = step->GetPostStepPoint();
      if (post->GetStepStatus() == fGeomBoundary && pre->GetMomentumDirection().x() <= 0) {
          track->SetTrackStatus(fStopAndKill);
      }
      return true;
    }

    track->SetTrackStatus(fStopAndKill);
    return true;
  }

  StepRecord rec;
  rec.t0       = pre->GetGlobalTime();
  rec.p0       = pre->GetPosition();
  rec.p1       = step->GetPostStepPoint()->GetPosition();
  rec.edep     = step->GetTotalEnergyDeposit();
  rec.pdg      = track->GetDefinition()->GetPDGEncoding();
  rec.trackID  = track->GetTrackID();
  rec.momentum = pre->GetMomentum();
  rec.kineticE = pre->GetKineticEnergy();
  rec.totalE   = pre->GetTotalEnergy();

  fSteps.push_back(rec);

  if (GetName() == "EndDetector") {
    track->SetTrackStatus(fStopAndKill);
  }

  return true;
}


// Called at the end of each event
void DetectorSD::EndOfEvent(G4HCofThisEvent*)
{
  
  auto runMgr = G4RunManager::GetRunManager();
  auto ev = runMgr->GetCurrentEvent();
  if (!ev) return;

  G4String detName = GetName();
  if (detName == "KillBox") return;
  
  fTotalEventsProcessed++;

  if (!fSteps.empty()) {
    // TODO: sacar TEST fuera de aqui
    std::filesystem::path resultsDir = std::filesystem::path(BUILD_DIR) / "results";
    std::filesystem::path hepmcFilePath = resultsDir / ("test_" + detName + "_PARTICLEFLOW.hepmc");

    ExportHepMC3(
        fTotalEventsProcessed,
        fSteps,
        hepmcFilePath.string()
    );
  }
}


// Export recorded hits to HepMC3 format
void DetectorSD::ExportHepMC3(
    G4int globalEventID,
    const std::vector<StepRecord>& steps,
    const std::string& hepmcFilename
)
{
  if (!fWriterHepMC3) {
    std::filesystem::path p(hepmcFilename);
    if (p.has_parent_path()) {
      std::filesystem::create_directories(p.parent_path());
    }

    fWriterHepMC3 = std::make_unique<HepMC3::WriterAscii>(hepmcFilename);
  }

  HepMC3::GenEvent evt(HepMC3::Units::MEV, HepMC3::Units::MM);
  evt.set_event_number(globalEventID);

  auto particleTable = G4ParticleTable::GetParticleTable();

  for (const auto& s : steps) {
    auto particle = particleTable->FindParticle(s.pdg);
    if (!particle) continue;

    const double mass = particle->GetPDGMass();

    const double px = s.momentum.x();
    const double py = s.momentum.y();
    const double pz = s.momentum.z();

    const double p2 = px*px + py*py + pz*pz;
    const double etot = std::sqrt(p2 + mass*mass);

    auto vtx = std::make_shared<HepMC3::GenVertex>(
      HepMC3::FourVector(
        s.p0.x()/mm,
        s.p0.y()/mm,
        s.p0.z()/mm,
        s.t0/ns
      )
    );

    auto outgoing = std::make_shared<HepMC3::GenParticle>(
      HepMC3::FourVector(px, py, pz, etot),
      s.pdg,
      1
    );

    vtx->add_particle_out(outgoing);
    evt.add_vertex(vtx);
  }

  fWriterHepMC3->write_event(evt);
}
