#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC3.h"   // contains Pythia8ToHepMC3
#include "HepMC3/WriterAscii.h"
#include <filesystem> // to create /build/tmp

void runPythia(int nEvents, std::string pythiaConfigFile, std::string outputFile) {
    Pythia8::Pythia pythia;
    HepMC3::Pythia8ToHepMC3 toHepMC;

    pythia.readFile(pythiaConfigFile);

    std::filesystem::create_directories(std::filesystem::path(outputFile).parent_path());
    HepMC3::WriterAscii writer(outputFile);

    if (!pythia.init()) return;

    for (short i = 0; i < nEvents; i++) {
        if (!pythia.next()) continue;

        HepMC3::GenEvent evt;
        toHepMC.fill_next_event(pythia, &evt);
        writer.write_event(evt);
    }
    writer.close();
}


void protonGun(int nEvents, double E_GeV, std::string outputFile) {

    std::filesystem::create_directories(std::filesystem::path(outputFile).parent_path());
    HepMC3::WriterAscii writer(outputFile);

    double m  = 0.938272;  // masa protón en GeV
    double pz = std::sqrt(E_GeV*E_GeV - m*m);

    for (int i = 0; i < nEvents; i++) {
        HepMC3::GenEvent evt(HepMC3::Units::GEV, HepMC3::Units::MM);
        evt.set_event_number(i);

        // Partícula de beam entrante (status 4 = beam particle)
        auto beam = std::make_shared<HepMC3::GenParticle>(
            HepMC3::FourVector(pz, 0, 0, E_GeV), 2212, 4);

        // Partícula saliente estable (status 1 = final state)
        auto proton = std::make_shared<HepMC3::GenParticle>(
            HepMC3::FourVector(pz, 0, 0, E_GeV), 2212, 1);

        // Vértice que conecta beam → salida
        auto v = std::make_shared<HepMC3::GenVertex>(
            HepMC3::FourVector(0, 0, 0, 0));
        v->add_particle_in(beam);
        v->add_particle_out(proton);
        evt.add_vertex(v);

        writer.write_event(evt);
    }
    writer.close();
}


/*
void protonGun(int nEvents, double E_GeV, std::string outputFile) {

    std::filesystem::create_directories(std::filesystem::path(outputFile).parent_path());
    HepMC3::WriterAscii writer(outputFile);

    double m = 0.938272;
    double p = std::sqrt(E_GeV*E_GeV - m*m);

    // Dirección hacia el detector (29.74, -5.36, 8.19) normalizada
    double norm = std::sqrt(29.74*29.74 + 5.36*5.36 + 8.19*8.19);
    double px =  29.74 / norm * p;
    double py =  -5.36 / norm * p;
    double pz =   8.19 / norm * p;

    for (int i = 0; i < nEvents; i++) {
        HepMC3::GenEvent evt(HepMC3::Units::GEV, HepMC3::Units::MM);
        evt.set_event_number(i);

        auto beam = std::make_shared<HepMC3::GenParticle>(
            HepMC3::FourVector(px, py, pz, E_GeV), 2212, 4);

        auto proton = std::make_shared<HepMC3::GenParticle>(
            HepMC3::FourVector(px, py, pz, E_GeV), 2212, 1);

        // Vértice en el IP (origen)
        auto v = std::make_shared<HepMC3::GenVertex>(
            HepMC3::FourVector(0, 0, 0, 0));  // en MM
        v->add_particle_in(beam);
        v->add_particle_out(proton);
        evt.add_vertex(v);

        writer.write_event(evt);
    }
    writer.close();
}

*/