#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "geant4/geometries/GeometryTypes.hh"
#include "config/ConfigParser.hh"

#include <vector>

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  explicit DetectorConstruction(const config::Geant4Config& geant4Config);
  ~DetectorConstruction() override = default;

  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;

private:
  config::Geant4Config m_geant4Config;
  std::vector<SensitiveVolumeInfo> fSensitiveVolumes;
};

#endif