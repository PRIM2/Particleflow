#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

#include "config/ConfigParser.hh"

#include <string>

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization() = default;

  ActionInitialization(const std::string& hepmcFile)
      : m_hepmcFile(hepmcFile) {}

  ActionInitialization(
      const std::string& hepmcFile,
      const config::Geant4Config& cfg
  )
      : m_hepmcFile(hepmcFile),
        m_cfg(cfg) {}

  ~ActionInitialization() override = default;

  void Build() const override;
  void BuildForMaster() const override;

private:
    std::string m_hepmcFile;
    config::Geant4Config m_cfg;
};

#endif