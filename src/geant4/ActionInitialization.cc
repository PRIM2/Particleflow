#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"

void ActionInitialization::Build() const {
    std::string filepath = m_hepmcFile.empty() 
        ? std::string(BUILD_DIR) + "/results/test_PYTHIA.hepmc"
        : m_hepmcFile;

    std::ifstream f(filepath);
    if (f.good())
        std::cout << "[OK] Fichero encontrado: " << filepath << std::endl;
    else
        std::cerr << "[ERROR] Fichero NO encontrado: " << filepath << std::endl;

    SetUserAction(new PrimaryGeneratorAction(filepath));
}

void ActionInitialization::BuildForMaster() const {
  // Vacío de momento (útil en MT para RunAction)
}

