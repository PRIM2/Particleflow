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

// HADRONES (los más usados)
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


#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

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
