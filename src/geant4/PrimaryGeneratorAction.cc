// LEPTONES
// - e-        (PDG 11)   electrón
// - e+        (PDG -11)  positrón
// - mu-       (PDG 13)   muón negativo
// - mu+       (PDG -13)  muón positivo
// - tau-      (PDG 15)   tau-
// - tau+      (PDG -15)  tau+

// FOTONES / NEUTRAS LIGERAS
// - gamma     (PDG 22)   fotón
// - neutrinoe (PDG 12)   neutrino electrónico (a veces: nu_e según build/tabla)
// - anti_neutrinoe (PDG -12) antineutrino e-
// - neutrino_mu (PDG 14), anti_neutrino_mu (PDG -14)
// - neutrino_tau (PDG 16), anti_neutrino_tau (PDG -16)

// HADRONES
// - proton    (PDG 2212) protón
// - anti_proton (PDG -2212)
// - neutron   (PDG 2112) neutrón
// - pi+       (PDG 211)  pión +
// - pi-       (PDG -211) pión -
// - pi0       (PDG 111)  pión 0
// - kaon+     (PDG 321)  K+
// - kaon-     (PDG -321) K-
// - kaon0L    (PDG 130)  K0 largo
// - kaon0S    (PDG 310)  K0 corto

// IONES / ALFAS (si tienes módulo ions)
// - alpha     (He4, PDG 1000020040)  partícula alfa
// - deuteron  (PDG 1000010020)
// - triton    (PDG 1000010030)
// - He3       (PDG 1000020030)

// ÓPTICA (si haces optical photons)
// - opticalphoton (no es "FindParticle" típico; se usa en óptica)

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4SystemOfUnits.hh"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"

// Constructor — abre el fichero
PrimaryGeneratorAction::PrimaryGeneratorAction(const std::string& filename)
  : m_reader(filename) {
  if (m_reader.failed())
    G4cerr << "ERROR: no se pudo abrir " << filename << G4endl;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  m_reader.close();
}

// Se llama UNA VEZ POR EVENTO -> introduce particles from HepMC3(pythia)
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* g4event) {

  // 1. Leer el siguiente evento del fichero
  HepMC3::GenEvent hepEvent;
  m_reader.read_event(hepEvent);
  if (m_reader.failed()) {
    G4cerr << "No hay mas eventos en el fichero .hepmc" << G4endl;
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
          if (particle->status() != 1) continue;
          auto mom = particle->momentum();
          G4PrimaryParticle* g4particle = new G4PrimaryParticle(
              particle->pid(),
              mom.px()*MeV, mom.py()*MeV, mom.pz()*MeV);

          if (!first) first = g4particle;
          else last->SetNext(g4particle);
          last = g4particle;
      }

      if (first) g4vertex->SetPrimary(first);
      g4event->AddPrimaryVertex(g4vertex);
  }
}


// ************** Particle GUN ***************
/* 
PrimaryGeneratorAction::PrimaryGeneratorAction() {
  fGun = new G4ParticleGun(1);

  fGun->SetParticleEnergy(20.*GeV);
  fGun->SetParticleMomentumDirection({0.,0.,-1.});
  fGun->SetParticlePosition({0.,0.,3.5*m});
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  delete fGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  auto table = G4ParticleTable::GetParticleTable();

  // 1) Neutrón
  // fGun->SetParticleDefinition(table->FindParticle("neutron"));
  // fGun->GeneratePrimaryVertex(event);

  // // 2) Muón (elige mu- o mu+)
  fGun->SetParticleDefinition(table->FindParticle("mu-"));
  fGun->GeneratePrimaryVertex(event);

  // // 3) Kaón (elige kaon+ o kaon-)
  // fGun->SetParticleDefinition(table->FindParticle("kaon+"));
  // fGun->GeneratePrimaryVertex(event);
}
*/