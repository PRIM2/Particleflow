// PrimaryGeneratorAction.cc
#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4SystemOfUnits.hh"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"

// Constructor
PrimaryGeneratorAction::PrimaryGeneratorAction(const std::string& filename): m_reader(filename) {
  if (m_reader.failed())
    G4cerr << "ERROR: it couldnt be opened " << filename << G4endl;
}

// Destructor
PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  m_reader.close();
}

// Se llama UNA VEZ POR EVENTO -> introduce particles from HepMC3(pythia)
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* g4event) {

  // 1. Leer el siguiente evento del fichero
  HepMC3::GenEvent hepEvent;
  m_reader.read_event(hepEvent);
  if (m_reader.failed()) {
    G4cerr << "There arent more evens in the .hepmc" << G4endl;
    return;
  }

  // 2. Recorrer los vértices del evento
  for (auto& vertex : hepEvent.vertices()) {
      auto pos = vertex->position();
      G4PrimaryVertex* g4vertex = new G4PrimaryVertex(
          pos.x(), pos.y(), pos.z(), pos.t());

      G4PrimaryParticle* first = nullptr;
      G4PrimaryParticle* last  = nullptr;

      for (auto& particle : vertex->particles_out()) {
          //if (particle->status() != 1) continue;
          auto mom = particle->momentum();
          G4PrimaryParticle* g4particle = new G4PrimaryParticle(
              particle->pid(),
              mom.px()*GeV, mom.py()*GeV, mom.pz()*GeV);

          if (!first) first = g4particle;
          else last->SetNext(g4particle);
          last = g4particle;
      }

      if (first) g4vertex->SetPrimary(first);
      g4event->AddPrimaryVertex(g4vertex);
  }
}

