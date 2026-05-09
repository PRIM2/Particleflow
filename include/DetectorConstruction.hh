#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include <vector>

class G4VPhysicalVolume;
class G4LogicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction() = default;
  ~DetectorConstruction() override = default;

  G4VPhysicalVolume* Construct() override;  // OBLIGATORIO
  void ConstructSDandField() override;
  float wallThickness;

  private:
    G4LogicalVolume* fLogicEndDet = nullptr;
    G4LogicalVolume* fLogicScintDet = nullptr;
    std::vector<G4LogicalVolume*> fLogicKillBoxFaces;
};

#endif
