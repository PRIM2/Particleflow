// DetectorConstrucion.cc

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

#include "DetectorConstruction.hh"

#include "G4SDManager.hh"
#include "GasSD.hh"

G4VPhysicalVolume* DetectorConstruction::Construct() {
  const int zAbsolute = -5;

  auto nist = G4NistManager::Instance();

  // World (aire) 3x3x3 m  -> semilados 1.5 m
  auto worldMat = nist->FindOrBuildMaterial("G4_AIR");
  auto solidWorld = new G4Box("World", 4*m, 4*m,10*m);
  auto logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");
  auto physWorld  = new G4PVPlacement(nullptr, {}, logicWorld, "World", nullptr, false, 0);


  // Muro de plomo (cara +X), espesor 10 cm
  auto pbMat = nist->FindOrBuildMaterial("G4_Pb");
  auto solidPbWall = new G4Box("PbWall", 3.5*m,3.5*m, 0.5*m);
  auto logicPbWall = new G4LogicalVolume(solidPbWall, pbMat, "PbWall");
  new G4PVPlacement(nullptr, 
                    G4ThreeVector(0, 0, (zAbsolute-1.5)*m), 
                    logicPbWall, "PbWall", 
                    logicWorld, 
                    false, 
                    0);


  // Muro de hormigón (cara +Y), espesor 10 cm
  auto conMat = nist->FindOrBuildMaterial("G4_CONCRETE");
  auto solidConWall = new G4Box("ConcreteWall", 3.5*m, 3.5*m, 50*cm); 
  auto logicConWall = new G4LogicalVolume(solidConWall, conMat, "ConcreteWall");
  new G4PVPlacement(nullptr, 
                    G4ThreeVector(0, 0, (zAbsolute+0)*m), 
                    logicConWall, 
                    "ConcreteWall", 
                    logicWorld, 
                    false, 
                    0);


  // --- Material: R-134a (C2H2F4), gas ---
  auto elC = nist->FindOrBuildElement("C");
  auto elH = nist->FindOrBuildElement("H");
  auto elF = nist->FindOrBuildElement("F");

  // Condiciones termodinámicas
  G4double temperature = 293.15*kelvin;      
  G4double pressure    = 1.*atmosphere;

  // Densidad aproximada del R-134a gas a 1 atm y 25 ºC
  G4double density     = 4.25*mg/cm3;  // miami me lo confirmo

  auto r134aMat = new G4Material("R134A", density, 3, kStateGas, temperature, pressure);
  r134aMat->AddElement(elC, 2);
  r134aMat->AddElement(elH, 2);
  r134aMat->AddElement(elF, 4);

  // Detector lleno de gas R-134a
  auto solidR134aDet = new G4Box("R134aDet", 3.5*m, 3.5*m, 20*cm);
  fLogicGasDet = new G4LogicalVolume(solidR134aDet, r134aMat, "R134aDet");
  new G4PVPlacement(nullptr,
                    G4ThreeVector(0., 0., (zAbsolute-3)*m),
                    fLogicGasDet,
                    "R134aDet",
                    logicWorld,
                    false,
                    0);
                    
  new G4PVPlacement(nullptr,
                  G4ThreeVector(0., 0., (zAbsolute-3.5)*m),
                  fLogicGasDet,
                  "R134aDet",
                  logicWorld,
                  false,
                  1);
                                  
              
  return physWorld;
}




void DetectorConstruction::ConstructSDandField()
{
  // Gestor de SDs de Geant4
  auto sdManager = G4SDManager::GetSDMpointer();

  // Creamos nuestro sensitive detector para el gas
  auto gasSD = new GasSD("GasSD");
  sdManager->AddNewDetector(gasSD);

  // Asignamos el SD al volumen lógico del gas
  if (fLogicGasDet) {
    fLogicGasDet->SetSensitiveDetector(gasSD);
  }
}
