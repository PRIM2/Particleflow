#pragma once

#include "geant4/geometries/GeometryScene.hh"

struct GeometryRealisticMapParams {
  double concreteHalfX_m = 1.75;
  double frontPbHalfZ_m  = 1.25;
};

class GeometryRealisticMap {
public:
  explicit GeometryRealisticMap(const GeometryRealisticMapParams& params = {});

  GeometryResult Construct();

private:
  GeometryRealisticMapParams fParams;
};