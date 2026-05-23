#include "geant4/geometries/GeometryRealistic.hh"
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


GeometryResult GeometryRealistic::Construct() {

  GeometryResult result;

  // Materials
  auto nist = G4NistManager::Instance();
  auto worldMat  = nist->FindOrBuildMaterial("G4_Galactic");
  auto concreteMat = nist->FindOrBuildMaterial("G4_CONCRETE");
  auto scintMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  auto pbMat = nist->FindOrBuildMaterial("G4_Pb");


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


  // *** End detector ***
  {
    // Parameters
    G4ThreeVector endDetHalfSize(20*cm, 1.09*m, 1.14*m);
    G4ThreeVector endDetPos(29.74142*m + 20*cm, -4.26804*m, 9.33062*m);
    G4Colour endDetColour(0.0, 0.5, 1.0, 0.2);
    G4String endDetName = "RPC";

    // Geometry
    auto solidEndDet = new G4Box(
      endDetName,
      endDetHalfSize.x(),
      endDetHalfSize.y(),
      endDetHalfSize.z()
    );
    auto logicEndDet = new G4LogicalVolume(solidEndDet, worldMat, endDetName);

    result.sensitiveVolumes.push_back({
      logicEndDet,
      DetectorType::RPC,
      endDetName
    });

    // Visualization attributes
    auto endDetAtt = new G4VisAttributes(endDetColour);
    endDetAtt->SetForceSolid(true);
    logicEndDet->SetVisAttributes(endDetAtt);

    // Placement
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
  

  // *** Concrete wall ***
  {
    // Parameters
    G4ThreeVector concWallHalfSize(1.75*m, 1.09*m, 1.14*m);
    G4ThreeVector concWallPos(29.74142*m - 1.75*m, -4.26804*m, 9.33062*m);
    G4Colour concWallColour(0.8, 0.0, 0.2, 0.2);
    G4String concWallName = "ConcreteWall";

    // Geometry
    auto solidConcWall = new G4Box(
      concWallName,
      concWallHalfSize.x(),
      concWallHalfSize.y(),
      concWallHalfSize.z()
    );
    auto logicConcWall = new G4LogicalVolume(solidConcWall, concreteMat, concWallName);
    result.sensitiveVolumes.push_back({ logicConcWall, DetectorType::Generic, concWallName });

    // Visualization attributes
    auto concWallAtt = new G4VisAttributes(concWallColour);
    concWallAtt->SetForceSolid(true);
    logicConcWall->SetVisAttributes(concWallAtt);

    // Placement
    new G4PVPlacement(
        nullptr,
        concWallPos,
        logicConcWall,
        concWallName,
        logicWorld,
        false,
        0
    );
  }


  // *** Front Pb trap ***
  {
    // Parameters
    G4double frontPbSmallHalfX = 50*cm;
    G4double frontPbLargeHalfX = 200*cm;
    G4double frontPbSmallHalfY = 50*cm;
    G4double frontPbLargeHalfY = 200*cm;
    G4double frontPbHalfZ = 1.25*m;

    G4ThreeVector frontPbPos(2.25*m, -0.34307*m, 0.75001*m);
    G4Colour frontPbColour(0.10, 0.22, 0.55, 0.65);
    G4String frontPbName = "FrontPbTrap";

    // Geometry
    auto solidFrontPb = new G4Trd(
      frontPbName,
      frontPbSmallHalfX,  // semi-X in -Z: small face
      frontPbLargeHalfX,  // semi-X in +Z: large face
      frontPbSmallHalfY,  // semi-Y in -Z
      frontPbLargeHalfY,  // semi-Y in +Z
      frontPbHalfZ        // semi-length in Z
    );

    auto logicFrontPb = new G4LogicalVolume(
      solidFrontPb,
      pbMat,
      frontPbName
    );
    result.sensitiveVolumes.push_back({ logicFrontPb,DetectorType::Generic, frontPbName });


    // Visualization attributes
    auto frontPbAtt = new G4VisAttributes(frontPbColour);
    frontPbAtt->SetForceSolid(true);
    logicFrontPb->SetVisAttributes(frontPbAtt);

    // Rotation
    auto rotFrontPb = new G4RotationMatrix();
    rotFrontPb->rotateY(-90*deg);

    // Placement
    new G4PVPlacement(
        rotFrontPb,
        frontPbPos,
        logicFrontPb,
        frontPbName,
        logicWorld,
        false,
        0
    );
  }

  
  // *** Scintillator detector ***
  {
    // Parameters
    G4ThreeVector scintDetHalfSize(1*cm, 2.0*m, 2.0*m);
    G4ThreeVector scintDetPos(3.5*m + 1*cm, -0.34307*m, 0.75001*m);
    G4Colour scintDetColour(0.35, 0.60, 0.95, 0.65);
    G4String scintDetName = "ScintDet";

    // Geometry
    auto solidScintDet = new G4Box(
      scintDetName,
      scintDetHalfSize.x(),
      scintDetHalfSize.y(),
      scintDetHalfSize.z()
    );

    auto logicScintDet = new G4LogicalVolume(solidScintDet, scintMat, scintDetName);

    result.sensitiveVolumes.push_back({
      logicScintDet,
      DetectorType::Scintillator,
      scintDetName
    });

    // Visualization attributes
    auto scintDetAtt = new G4VisAttributes(scintDetColour);
    scintDetAtt->SetForceSolid(true);
    logicScintDet->SetVisAttributes(scintDetAtt);

    // Placement
    new G4PVPlacement(
        nullptr,
        scintDetPos,
        logicScintDet,
        scintDetName,
        logicWorld,
        false,
        0
    );
  }


  // *** Back Pb trap ***
  {
    // Parameters
    G4double backPbSmallHalfX = 2.0*m;
    G4double backPbLargeHalfX = 2.5*m;
    G4double backPbSmallHalfY = 2.0*m;
    G4double backPbLargeHalfY = 2.5*m;
    G4double backPbHalfZ = 0.5*m;

    G4ThreeVector backPbPos(4.02*m, -0.34307*m, 0.75001*m);
    G4Colour backPbColour(0.35, 0.60, 0.95, 0.65);
    G4String backPbName = "BackPbTrap";

    // Geometry
    auto solidBackPb = new G4Trd(
      backPbName,
      backPbSmallHalfX,  // semi-X in -Z: small face
      backPbLargeHalfX,  // semi-X in +Z: large face
      backPbSmallHalfY,  // semi-Y in -Z
      backPbLargeHalfY,  // semi-Y in +Z
      backPbHalfZ        // semi-length in Z
    );

    auto logicBackPb = new G4LogicalVolume(
      solidBackPb,
      pbMat,
      backPbName
    );

    result.sensitiveVolumes.push_back({ logicBackPb, DetectorType::Generic, backPbName });


    // Visualization attributes
    auto backPbAtt = new G4VisAttributes(backPbColour);
    backPbAtt->SetForceSolid(true);
    logicBackPb->SetVisAttributes(backPbAtt);

    // Rotation
    auto rotBackPb = new G4RotationMatrix();
    rotBackPb->rotateY(-90*deg);

    // Placement
    new G4PVPlacement(
        rotBackPb,
        backPbPos,
        logicBackPb,
        backPbName,
        logicWorld,
        false,
        0
    );
  }


  // *** Kill box faces *** 
  {
    // Parameters
    G4ThreeVector killBoxHalfSize(2.35*m, 2.70*m, 2.70*m);
    G4ThreeVector killBoxPos(3.15*m, -0.34307*m, 0.75001*m);
    G4double killBoxFaceThick = 1*mm;
    G4Colour killBoxColour(1.0, 0.0, 0.0, 1.0);

    // -X face
    G4String killBoxName1 = "KillBox_MX";

    // Geometry
    auto solidKillBox1 = new G4Box(
      killBoxName1,
      killBoxFaceThick,
      killBoxHalfSize.y(),
      killBoxHalfSize.z()
    );
    auto logicKillBox1 = new G4LogicalVolume(
      solidKillBox1,
      worldMat,
      killBoxName1
    );

    result.sensitiveVolumes.push_back({
      logicKillBox1,
      DetectorType::KillBox,
      killBoxName1
    });

    // Visualization attributes
    auto killBoxAtt1 = new G4VisAttributes(killBoxColour);
    killBoxAtt1->SetForceWireframe(true);
    killBoxAtt1->SetForceAuxEdgeVisible(true);
    logicKillBox1->SetVisAttributes(killBoxAtt1);

    // Placement
    new G4PVPlacement(
        nullptr,
        G4ThreeVector(
          killBoxPos.x() - killBoxHalfSize.x(),
          killBoxPos.y(),
          killBoxPos.z()
        ),
        logicKillBox1,
        killBoxName1,
        logicWorld,
        false,
        0
    );


    // +Y face
    G4String killBoxName2 = "KillBox";

    // Geometry
    auto solidKillBox2 = new G4Box(
      killBoxName2,
      killBoxHalfSize.x(),
      killBoxFaceThick,
      killBoxHalfSize.z()
    );
    auto logicKillBox2 = new G4LogicalVolume(
      solidKillBox2,
      worldMat,
      killBoxName2
    );

    result.sensitiveVolumes.push_back({
      logicKillBox2,
      DetectorType::KillBox,
      killBoxName2
    });

    // Visualization attributes
    auto killBoxAtt2 = new G4VisAttributes(killBoxColour);
    killBoxAtt2->SetForceWireframe(true);
    killBoxAtt2->SetForceAuxEdgeVisible(true);
    logicKillBox2->SetVisAttributes(killBoxAtt2);

    // Placement
    new G4PVPlacement(
        nullptr,
        G4ThreeVector(
          killBoxPos.x(),
          killBoxPos.y() + killBoxHalfSize.y(),
          killBoxPos.z()
        ),
        logicKillBox2,
        killBoxName2,
        logicWorld,
        false,
        0
    );


    // -Y face
    G4String killBoxName3 = "KillBox";

    // Geometry
    auto solidKillBox3 = new G4Box(
      killBoxName3,
      killBoxHalfSize.x(),
      killBoxFaceThick,
      killBoxHalfSize.z()
    );
    auto logicKillBox3 = new G4LogicalVolume(
      solidKillBox3,
      worldMat,
      killBoxName3
    );

    result.sensitiveVolumes.push_back({
      logicKillBox3,
      DetectorType::KillBox,
      killBoxName3
    });

    // Visualization attributes
    auto killBoxAtt3 = new G4VisAttributes(killBoxColour);
    killBoxAtt3->SetForceWireframe(true);
    killBoxAtt3->SetForceAuxEdgeVisible(true);
    logicKillBox3->SetVisAttributes(killBoxAtt3);

    // Placement
    new G4PVPlacement(
        nullptr,
        G4ThreeVector(
          killBoxPos.x(),
          killBoxPos.y() - killBoxHalfSize.y(),
          killBoxPos.z()
        ),
        logicKillBox3,
        killBoxName3,
        logicWorld,
        false,
        0
    );


    // +Z face
    G4String killBoxName4 = "KillBox";

    // Geometry
    auto solidKillBox4 = new G4Box(
      killBoxName4,
      killBoxHalfSize.x(),
      killBoxHalfSize.y(),
      killBoxFaceThick
    );
    auto logicKillBox4 = new G4LogicalVolume(
      solidKillBox4,
      worldMat,
      killBoxName4
    );

    result.sensitiveVolumes.push_back({
      logicKillBox4,
      DetectorType::KillBox,
      killBoxName4
    });

    // Visualization attributes
    auto killBoxAtt4 = new G4VisAttributes(killBoxColour);
    killBoxAtt4->SetForceWireframe(true);
    killBoxAtt4->SetForceAuxEdgeVisible(true);
    logicKillBox4->SetVisAttributes(killBoxAtt4);

    // Placement
    new G4PVPlacement(
        nullptr,
        G4ThreeVector(
          killBoxPos.x(),
          killBoxPos.y(),
          killBoxPos.z() + killBoxHalfSize.z()
        ),
        logicKillBox4,
        killBoxName4,
        logicWorld,
        false,
        0
    );


    // -Z face
    G4String killBoxName5 = "KillBox";

    // Geometry
    auto solidKillBox5 = new G4Box(
      killBoxName5,
      killBoxHalfSize.x(),
      killBoxHalfSize.y(),
      killBoxFaceThick
    );
    auto logicKillBox5 = new G4LogicalVolume(
      solidKillBox5,
      worldMat,
      killBoxName5
    );

    result.sensitiveVolumes.push_back({
      logicKillBox5,
      DetectorType::KillBox,
      killBoxName5
    });

    // Visualization attributes
    auto killBoxAtt5 = new G4VisAttributes(killBoxColour);
    killBoxAtt5->SetForceWireframe(true);
    killBoxAtt5->SetForceAuxEdgeVisible(true);
    logicKillBox5->SetVisAttributes(killBoxAtt5);

    // Placement
    new G4PVPlacement(
        nullptr,
        G4ThreeVector(
          killBoxPos.x(),
          killBoxPos.y(),
          killBoxPos.z() - killBoxHalfSize.z()
        ),
        logicKillBox5,
        killBoxName5,
        logicWorld,
        false,
        0
    );
  }

  
  // *** IPS marker ***
  {
    // Parameters
    G4ThreeVector cubeHalfSize(0.25*m, 0.25*m, 0.25*m);
    G4ThreeVector cubePos(0, 0, 0);
    G4Colour cubeColour(0.0, 1.0, 0.0, 0.35);
    G4String cubeName = "Cube";

    // Geometry
    auto solidCube = new G4Box(
      cubeName,
      cubeHalfSize.x(),
      cubeHalfSize.y(),
      cubeHalfSize.z()
    );
    auto logicCube = new G4LogicalVolume(
      solidCube,
      worldMat,
      cubeName
    );

    // Visualization attributes
    auto cubeAtt = new G4VisAttributes(cubeColour);
    cubeAtt->SetForceSolid(true);
    logicCube->SetVisAttributes(cubeAtt);

    // Placement
    new G4PVPlacement(
        nullptr,
        cubePos,
        logicCube,
        cubeName,
        logicWorld,
        false,
        0
    );
  }

  result.physWorld = physWorld;

  return result;
}