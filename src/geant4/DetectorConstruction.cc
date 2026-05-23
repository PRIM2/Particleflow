#include "geant4/DetectorConstruction.hh"
#include "geant4/geometries/AllGeometries.hh"
#include "geant4/detectorSDs/AllDetectorSD.hh"

#include "G4SDManager.hh"

#include <stdexcept>


DetectorConstruction::DetectorConstruction(const config::Geant4Config& geant4Config)
  : m_geant4Config(geant4Config)
{
}


G4VPhysicalVolume* DetectorConstruction::Construct() {
  fSensitiveVolumes.clear();

  GeometryResult result;

  if (m_geant4Config.geometry == "SimpleWall") {
    GeometrySimpleWall geometry;
    result = geometry.Construct();
  }
  else if (m_geant4Config.geometry == "Realistic") {
    GeometryRealistic geometry;
    result = geometry.Construct();
  }
  else if (m_geant4Config.geometry == "RealisticMap") {
    GeometryRealisticMapParams params;
    params.concreteHalfX_m = m_geant4Config.realisticMap.concreteHalfX;
    params.frontPbHalfZ_m  = m_geant4Config.realisticMap.frontPbHalfZ;

    GeometryRealisticMap geometry(params);
    result = geometry.Construct();
  }
  else {
    throw std::runtime_error(
      "Unknown geometry: " + m_geant4Config.geometry
    );
  }

  fSensitiveVolumes = result.sensitiveVolumes;

  return result.physWorld;
}


void DetectorConstruction::ConstructSDandField() {
  auto sdManager = G4SDManager::GetSDMpointer();

  BaseSD::SetOutputFile(m_geant4Config.output);

  for (const auto& sensitive : fSensitiveVolumes) {
    if (!sensitive.logicVolume) {
      continue;
    }

    G4VSensitiveDetector* detector = nullptr;

    switch (sensitive.detectorType) {
      case DetectorType::Generic:
        detector = new GenericSD(sensitive.detectorName);
        break;

      case DetectorType::Scintillator:
        detector = new ScintillatorSD(sensitive.detectorName);
        break;

      case DetectorType::KillBox:
        detector = new KillBoxSD(sensitive.detectorName);
        break;

      case DetectorType::RPC:
        detector = new RPCSD(sensitive.detectorName);
        break;

      case DetectorType::None:
        continue;
    }

    sdManager->AddNewDetector(detector);
    sensitive.logicVolume->SetSensitiveDetector(detector);
  }
}