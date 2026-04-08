#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include <string>

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization() = default;
  ActionInitialization(const std::string& hepmcFile) : m_hepmcFile(hepmcFile) {}
  ~ActionInitialization() override = default;

  void Build() const override;
  void BuildForMaster() const override;

private:
    std::string m_hepmcFile;
};

#endif