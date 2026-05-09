#pragma once
#include <string>

class G4Simulator {
public:
    G4Simulator(int nEvents,
                const std::string& hepmcFile,
                bool uiMode = false,
                double thickness = 0.5);
    ~G4Simulator();

    void run(int argc, char** argv);

private:
    int         m_nEvents;
    std::string m_hepmcFile;
    bool        m_uiMode;
    double      m_thickness;
};