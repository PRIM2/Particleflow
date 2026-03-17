#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC3.h"   // contains Pythia8ToHepMC3
#include "HepMC3/WriterAscii.h"
#include <filesystem> // to create /build/tmp

void pruebasPythia(int nEvents, std::string pythiaConfigFile) {
    Pythia8::Pythia pythia;
    HepMC3::Pythia8ToHepMC3 toHepMC;

    pythia.readFile(std::string(PROJECT_DIR) + "/cmndPythia/" + pythiaConfigFile);

    std::string outputDir = std::string(BUILD_DIR) + "/tmp";
    std::filesystem::create_directories(outputDir); //build por si hay varios binarios... 
    HepMC3::WriterAscii writer(outputDir + "/" + "test.hepmc");

    if (!pythia.init()) return;

    for (short i = 0; i < nEvents; i++) {
        if (!pythia.next()) continue;

        /*  HepMC3 CONVERSOR*/
        HepMC3::GenEvent evt;                          // create event
        toHepMC.fill_next_event(pythia, &evt);         // fill event (Convert Pythia → HepMC)
        writer.write_event(evt);                       // write event into file
        
    }
    writer.close();

}

short prueba() {
    // THIS WILL BE ELIMINATED, test
    return 1010;
}