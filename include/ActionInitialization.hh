#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization() = default;
  ~ActionInitialization() override = default;

  void Build() const override;             // OBLIGATORIO
  void BuildForMaster() const override;    // recomendable
};

#endif
