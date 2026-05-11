#include "geant4/geometries/GeometryScene.hh"

// Geant 4 
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ThreeVector.hh"
#include "G4String.hh"
#include "G4RotationMatrix.hh"


GeometryResult GeometryScene::Construct() {

  GeometryResult result;

  // Materials
  auto nist = G4NistManager::Instance();
  auto worldMat  = nist->FindOrBuildMaterial("G4_Galactic");


  // World variables used outside local scope
  G4ThreeVector worldHalfSize;
  G4LogicalVolume* logicWorld = nullptr;
  G4VPhysicalVolume* physWorld = nullptr;
  // *** World ***
  {
    // Parameters
    worldHalfSize = G4ThreeVector(31*m, 6*m, 11.5*m);
    G4String worldName = "World";

    // Geometry
    auto solidWorld = new G4Box(
      worldName,
      worldHalfSize.x(),
      worldHalfSize.y(),
      worldHalfSize.z()
    );

    logicWorld = new G4LogicalVolume(
      solidWorld,
      worldMat,
      worldName
    );

    // Visualization attributes
    logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

    // Placement
    physWorld = new G4PVPlacement(
        nullptr,
        G4ThreeVector(),
        logicWorld,
        worldName,
        nullptr,
        false,
        0
    );
  }


  // *** Decorative external faces ***
  {
    G4double cutNegX = 28 * m;   // 0 all the world, cutNegX==LX middle world 
    G4double faceThick = 1*mm;  

    auto createFace = [&](
      G4String name, 
      G4double dx, G4double dy, G4double dz, 
      G4ThreeVector pos, G4Colour col
    ) {
        auto box = new G4Box(name, dx, dy, dz);
        auto log = new G4LogicalVolume(box, worldMat, name);
        auto att = new G4VisAttributes(col);
        att->SetForceSolid(true);
        log->SetVisAttributes(att);
        new G4PVPlacement(nullptr, pos, log, name, logicWorld, false, 0);
    };

    G4double centerX = cutNegX / 2.0;
    G4double halfX   = worldHalfSize.x()- cutNegX / 2.0;

    // +X
    createFace(
      "F1", faceThick, worldHalfSize.y(), worldHalfSize.z(), 
      G4ThreeVector( worldHalfSize.x()- faceThick, 0, 0),
      G4Colour(0.38, 0.40, 0.44, 0.30)
    );

    // -X
    createFace(
      "F2", faceThick, worldHalfSize.y(), worldHalfSize.z(),
      G4ThreeVector(-worldHalfSize.x()+ cutNegX + faceThick, 0, 0),
      G4Colour(0.74, 0.77, 0.82, 0.1)
    );

    // +Z  transparent (in front of me)
    createFace(
      "F3", halfX, worldHalfSize.y(), faceThick,
      G4ThreeVector(centerX, 0, worldHalfSize.z() - faceThick),
      G4Colour(1.0, 1.0, 1.0, 0.00)
    );

    // -Z
    createFace(
      "F4", halfX, worldHalfSize.y(), faceThick,
      G4ThreeVector(centerX, 0, -worldHalfSize.z() + faceThick),
      G4Colour(0.56, 0.59, 0.64, 0.18)
    );

    // +Y
    createFace(
      "F5", halfX, faceThick, worldHalfSize.z(),
      G4ThreeVector(centerX, worldHalfSize.y() - faceThick, 0),
      G4Colour(0.84, 0.86, 0.90, 0.2)
    );

    // -Y
    createFace(
      "F6", halfX, faceThick, worldHalfSize.z(),
      G4ThreeVector(centerX, -worldHalfSize.y() + faceThick, 0),
      G4Colour(0.34, 0.36, 0.40, 0.20)
    );
  }


  result.physWorld = physWorld;
  result.logicWorld = logicWorld;

  return result;
}