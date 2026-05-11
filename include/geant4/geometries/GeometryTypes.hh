#pragma once

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4String.hh"

#include <vector>


enum class GeometryType {
  SimpleWall,
  Realistic,
  Scene
};

enum class DetectorType {
  Generic,
  Scintillator,
  KillBox,
  RPC,
  None
};


struct SensitiveVolumeInfo {
  G4LogicalVolume* logicVolume = nullptr;
  DetectorType detectorType;
  G4String detectorName;
};


struct GeometryResult {
  G4VPhysicalVolume* physWorld = nullptr;
  G4LogicalVolume* logicWorld = nullptr;
  std::vector<SensitiveVolumeInfo> sensitiveVolumes;
};
