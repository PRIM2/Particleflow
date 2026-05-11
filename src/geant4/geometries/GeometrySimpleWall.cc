#include "geant4/geometries/GeometrySimpleWall.hh"
#include "geant4/geometries/GeometryScene.hh"


// Geant 4 
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4Colour.hh"
#include "G4ThreeVector.hh"
#include "G4String.hh"



GeometryResult GeometrySimpleWall::Construct() {

  GeometryScene scene;
  GeometryResult result = scene.Construct();

  G4LogicalVolume* logicWorld = result.logicWorld;

  auto nist = G4NistManager::Instance();
  auto airMat = nist->FindOrBuildMaterial("G4_AIR");

  {
    G4ThreeVector endDetHalfSize(20*cm, 5*m, 5*m);
    G4ThreeVector endDetPos(15*m, 0, 0);
    G4Colour endDetColour(0.0, 0.5, 1.0, 0.2);
    G4String endDetName = "RPC";

    auto solidEndDet = new G4Box(
      endDetName,
      endDetHalfSize.x(),
      endDetHalfSize.y(),
      endDetHalfSize.z()
    );

    auto logicEndDet = new G4LogicalVolume(
      solidEndDet,
      airMat,
      endDetName
    );

    result.sensitiveVolumes.push_back({
      logicEndDet,
      DetectorType::RPC,
      endDetName
    });

    auto endDetAtt = new G4VisAttributes(endDetColour);
    endDetAtt->SetForceSolid(true);
    logicEndDet->SetVisAttributes(endDetAtt);

    new G4PVPlacement(
      nullptr,
      endDetPos,
      logicEndDet,
      endDetName,
      logicWorld,
      false,
      0
    );
  }

  return result;
}