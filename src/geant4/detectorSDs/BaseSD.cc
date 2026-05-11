// BaseSD.cc
#include "geant4/detectorSDs/BaseSD.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/FourVector.h"

#include <cmath>
#include <filesystem>
#include <stdexcept>

// TODO: revisar unidades importantisimo, energia tiempo y distancia

std::string BaseSD::sOutputFile = "";

BaseSD::BaseSD(const G4String& name)
  : G4VSensitiveDetector(name) {}

BaseSD::~BaseSD() {
  if (fWriterHepMC3) {
    fWriterHepMC3->close();
  }
}

void BaseSD::SetOutputFile(const std::string& outputFile) {
  sOutputFile = outputFile;
}

void BaseSD::Initialize(G4HCofThisEvent*) {
  fSteps.clear();
}

void BaseSD::RecordStep(G4Step* step) {
  if (!step) {
    return;
  }

  auto pre = step->GetPreStepPoint();
  auto post = step->GetPostStepPoint();
  auto track = step->GetTrack();

  if (!pre || !post || !track) {
    return;
  }

  StepRecord rec;

  rec.t0       = pre->GetGlobalTime();
  rec.p0       = pre->GetPosition();
  rec.p1       = post->GetPosition();
  rec.edep     = step->GetTotalEnergyDeposit();
  rec.pdg      = track->GetDefinition()->GetPDGEncoding();
  rec.trackID  = track->GetTrackID();
  rec.momentum = pre->GetMomentum();
  rec.kineticE = pre->GetKineticEnergy();
  rec.totalE   = pre->GetTotalEnergy();

  fSteps.push_back(rec);
}

void BaseSD::EndOfEvent(G4HCofThisEvent*) {
  auto runMgr = G4RunManager::GetRunManager();
  auto ev = runMgr->GetCurrentEvent();

  if (!ev) {
    return;
  }

  if (sOutputFile.empty()) {
    throw std::runtime_error("BaseSD output file has not been set.");
  }

  const G4int eventID = ev->GetEventID();

  std::filesystem::path basePath(sOutputFile);

  std::filesystem::path hepmcFilePath =
    basePath.parent_path() /
    (
      basePath.stem().string() +
      "_" +
      GetName() +
      basePath.extension().string()
    );

  ExportHepMC3(
    eventID,
    fSteps,
    hepmcFilePath.string()
  );
}

void BaseSD::ExportHepMC3(
  G4int globalEventID,
  const std::vector<StepRecord>& steps,
  const std::string& hepmcFilename
) {
  if (!fWriterHepMC3) {
    std::filesystem::path p(hepmcFilename);

    if (p.has_parent_path()) {
      std::filesystem::create_directories(p.parent_path());
    }

    fWriterHepMC3 = std::make_unique<HepMC3::WriterAscii>(hepmcFilename);
  } 

  HepMC3::GenEvent evt(HepMC3::Units::GEV, HepMC3::Units::MM);
  evt.set_event_number(globalEventID);

  auto particleTable = G4ParticleTable::GetParticleTable();

  for (const auto& s : steps) {
    auto particle = particleTable->FindParticle(s.pdg);

    if (!particle) {
      continue;
    }

    const double mass = particle->GetPDGMass() / GeV;

    const double px = s.momentum.x() / GeV;
    const double py = s.momentum.y() / GeV;
    const double pz = s.momentum.z() / GeV;

    const double p2 = px * px + py * py + pz * pz;
    const double etot = std::sqrt(p2 + mass * mass);

    auto vtx = std::make_shared<HepMC3::GenVertex>(
      HepMC3::FourVector(
        s.p0.x() / mm,
        s.p0.y() / mm,
        s.p0.z() / mm,
        s.t0 / ns
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