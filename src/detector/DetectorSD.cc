#include "DetectorSD.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHandle.hh"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/FourVector.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem> // to create /build/results directory

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


// Constructor: inicializamos el contador global a 0
DetectorSD::DetectorSD(const G4String& name)
  : G4VSensitiveDetector(name), fTotalEventsProcessed(0)
{}

// Destructor: IMPORTANTE cerrar el archivo para que se guarde bien
DetectorSD::~DetectorSD()
{
  if (fWriterHepMC3) {
    fWriterHepMC3->close();


    std::string resultsDir = std::string(BUILD_DIR) + "/results";
    // TODO: NO PUEDE ESTAR ASI DE MAL HECHO
    std::ofstream countFile(resultsDir + "/simulacion_acumulada.nevents");
    countFile << fTotalEventsProcessed << "\n";
  }
}

void DetectorSD::Initialize(G4HCofThisEvent* /*hce*/)
{
  fEdepTot = 0.;
  fSteps.clear(); // Limpiamos los pasos del evento anterior
}

G4bool DetectorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  auto pre = step->GetPreStepPoint();
  if (pre->GetStepStatus() != fGeomBoundary) return false;




  auto track = step->GetTrack();

  StepRecord rec;
  rec.t0       = pre->GetGlobalTime();
  rec.p0       = pre->GetPosition();
  rec.p1       = step->GetPostStepPoint()->GetPosition();
  rec.edep     = step->GetTotalEnergyDeposit();
  rec.pdg      = track->GetDefinition()->GetPDGEncoding();
  rec.trackID  = track->GetTrackID();
  rec.momentum = pre->GetMomentum();      // (px, py, pz) en MeV/c
  rec.kineticE = pre->GetKineticEnergy(); // MeV
  rec.totalE   = pre->GetTotalEnergy();   // Ekin + masa, en MeV

  fSteps.push_back(rec);
  fEdepTot += rec.edep;
  return true;
}




void DetectorSD::EndOfEvent(G4HCofThisEvent*)
{
  auto runMgr = G4RunManager::GetRunManager();
  auto ev = runMgr->GetCurrentEvent();
  if (!ev) return;

  // Incrementamos el contador global (sin importar el RunID)
  fTotalEventsProcessed++; 

  if (!fSteps.empty()) {
    // TODO sacar esto a los binarios (poner atributo en .hh y modificar en binario)
    std::string hepmcFilePath = std::string(BUILD_DIR) + "/results" + "/test_PARTICLEFLOW.hepmc";
    // Usamos el contador absoluto para identificar el evento en el archivo
    ExportHepMC3(fTotalEventsProcessed, fSteps, hepmcFilePath);
  }
}


void DetectorSD::ExportHepMC3(G4int globalEventID, const std::vector<StepRecord>& steps, const std::string& hepmcFilename)
{
  if (!fWriterHepMC3) {
    // Asegurar directorio y crear writer
    std::filesystem::path p(hepmcFilename);
    if (p.has_parent_path()) std::filesystem::create_directories(p.parent_path());
    fWriterHepMC3 = std::make_unique<HepMC3::WriterAscii>(hepmcFilename);
  }

  // Definir unidades (MeV y MM son estándar en Geant4 y recomendados en HepMC3)
  HepMC3::GenEvent evt(HepMC3::Units::MEV, HepMC3::Units::MM);
  evt.set_event_number(globalEventID);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  for (const auto& s : steps) {
    // 1. Obtener la masa de la partícula para calcular Energía Total
    G4ParticleDefinition* particle = particleTable->FindParticle(s.pdg);
    if (!particle) continue; // Saltear si el PDG es desconocido
    
    double mass = particle->GetPDGMass(); // Ya viene en MeV
    double px = s.momentum.x(); // Asumimos MeV
    double py = s.momentum.y();
    double pz = s.momentum.z();
    
    // 2. Calcular Energía Total: E = sqrt(p^2 + m^2)
    double p2 = px*px + py*py + pz*pz;
    double etot = std::sqrt(p2 + mass*mass);

    // 3. Crear Vértice en la posición de entrada (p0)
    auto vtx = std::make_shared<HepMC3::GenVertex>(
      HepMC3::FourVector(s.p0.x()/mm, s.p0.y()/mm, s.p0.z()/mm, s.t0/ns)
    );

    // 4. Crear Partícula de salida (Status 1: para ser simulada)
    // El cuatrimomento en HepMC3 es (px, py, pz, E)
    auto outgoing = std::make_shared<HepMC3::GenParticle>(
      HepMC3::FourVector(px, py, pz, etot), s.pdg, 1
    );

    vtx->add_particle_out(outgoing);
    evt.add_vertex(vtx);
  }

  fWriterHepMC3->write_event(evt);
}

/*
void DetectorSD::ExportHepMC3(G4int globalEventID, const std::vector<StepRecord>& steps, const std::string& hepmcFilename)
{
  if (!fWriterHepMC3) {
     std::filesystem::create_directories(hepmcFilename.substr(0, hepmcFilename.find_last_of("/")));
    fWriterHepMC3 = std::make_unique<HepMC3::WriterAscii>(hepmcFilename);
  }

  HepMC3::GenEvent evt(HepMC3::Units::MEV, HepMC3::Units::MM);
  evt.set_event_number(globalEventID);

  for (const auto& s : steps) {
    G4ThreeVector mid = 0.5 * (s.p0 + s.p1);
    auto vtx = std::make_shared<HepMC3::GenVertex>(
      HepMC3::FourVector(mid.x()/mm, mid.y()/mm, mid.z()/mm, s.t0/ns)
    );

    const double px = s.momentum.x() / MeV;
    const double py = s.momentum.y() / MeV;
    const double pz = s.momentum.z() / MeV;
    const double E  = s.kineticE / MeV;

    // Partícula entrante fantasma (status=4, ignorada por el plotter)
    auto incoming = std::make_shared<HepMC3::GenParticle>(
      HepMC3::FourVector(px, py, pz, E), s.pdg, 4
    );
    vtx->add_particle_in(incoming);

    // Partícula saliente = la que registra el plotter (status=1)
    auto outgoing = std::make_shared<HepMC3::GenParticle>(
      HepMC3::FourVector(px, py, pz, E), s.pdg, 1
    );
    vtx->add_particle_out(outgoing);

    evt.add_vertex(vtx);
  }

  fWriterHepMC3->write_event(evt);
}
*/
/*
void DetectorSD::ExportCSV(G4int eventID, G4int nTracks, const std::vector<StepRecord>& steps)
{

  // ****************************************************************************

  //TODO
  // ESTA MAL HECHO, NO DIFERENCIA ENTRE BEAMS, LO MEZCLA TODO EN EL MISMO ARCHIVO
  //PONER NUEVO ATRIBUTO QUE DIGA QUE BEAMON ES O SOUCIONARLO
  std::cout << "REVISAR CODE CSV, ESATA INCOMPLETO, MEZCLA TODO EN EL MISMO ARCHIVO, NO DIFERENCIA ENTRE BEAMS***********************************************" << std::endl;

  // ****************************************************************************

  // Crea el fichero solo la primera vez, escribe la cabecera
  if (!fWriterCSV) {
    std::filesystem::create_directories(std::string(BUILD_DIR) + "/results");
    fWriterCSV = std::make_unique<std::ofstream>(
      std::string(BUILD_DIR) + "/results/gas_output.dat"
    );
    // cabecera solo una vez
    *fWriterCSV << "t0[ns],x0[mm],y0[mm],z0[mm],x1[mm],y1[mm],z1[mm],"
                   "Edep[MeV],event_id,track_id,pdg,volume_name,volume_copyNo\n";
  }

  // Escribe los pasos de este evento
  fWriterCSV->precision(10);
  for (const auto& s : steps) {
    *fWriterCSV << (s.t0/ns) << ","
                << (s.p0.x()/mm) << "," << (s.p0.y()/mm) << "," << (s.p0.z()/mm) << ","
                << (s.p1.x()/mm) << "," << (s.p1.y()/mm) << "," << (s.p1.z()/mm) << ","
                << (s.edep/MeV) << ","
                << eventID << ","
                << s.trackID << ","
                << s.pdg << ","
                << s.volumeName << ","
                << s.volumeCopyNo << "\n";
  }
}
*/

