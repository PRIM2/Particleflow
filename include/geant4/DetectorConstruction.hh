#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "geant4/geometries/GeometryTypes.hh"

#include <string>
#include <vector>

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  explicit DetectorConstruction(const std::string& geometryName, const std::string& output);
  ~DetectorConstruction() override = default;

  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;

private:
  std::string m_geometryName;
  std::string m_output;
  std::vector<SensitiveVolumeInfo> fSensitiveVolumes;
};

#endif