// Collider.cc
#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC3.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/FourVector.h"
#include "HepMC3/WriterAscii.h"

#include <filesystem>
#include <cmath>
#include <memory>
#include <string>
#include <stdexcept>


void runPythia(int nEvents, std::string pythiaConfigFile, std::string outputFile, double beamEnergy) {
    Pythia8::Pythia pythia;
    HepMC3::Pythia8ToHepMC3 toHepMC;

    pythia.readFile(pythiaConfigFile);
    pythia.readString("Beams:eA = " + std::to_string(beamEnergy));
    pythia.readString("Beams:eB = " + std::to_string(beamEnergy));
    std::filesystem::create_directories(std::filesystem::path(outputFile).parent_path());
    HepMC3::WriterAscii writer(outputFile);

    if (!pythia.init()) return;

    for (int i = 0; i < nEvents; i++) {
        if (!pythia.next()) continue;

        HepMC3::GenEvent evt;
        toHepMC.fill_next_event(pythia, &evt);

        for (auto& p : evt.particles()) {
            auto m = p->momentum();
            p->set_momentum(HepMC3::FourVector(m.px(), m.py(), m.pz(), m.e()));
        }

        for (auto& v : evt.vertices()) {
            auto pos = v->position();
            v->set_position(HepMC3::FourVector(pos.x(), pos.y(), pos.z(), pos.t()));
        }
        writer.write_event(evt);
    }
    writer.close();
}



void particleGun(int nEvents, double E_GeV, std::string outputFile, int pdg, double mass_GeV) {
    if (E_GeV < mass_GeV) throw std::runtime_error("Particle energy must be greater than or equal to mass.");
    
    std::filesystem::create_directories(std::filesystem::path(outputFile).parent_path());

    HepMC3::WriterAscii writer(outputFile);

    double px = std::sqrt(E_GeV * E_GeV - mass_GeV * mass_GeV);

    for (int i = 0; i < nEvents; i++) {

        HepMC3::GenEvent evt(HepMC3::Units::GEV, HepMC3::Units::MM);

        evt.set_event_number(i);

        auto beam = std::make_shared<HepMC3::GenParticle>(
            HepMC3::FourVector(px, 0, 0, E_GeV),
            pdg,
            4
        );

        auto particle = std::make_shared<HepMC3::GenParticle>(
            HepMC3::FourVector(px, 0, 0, E_GeV),
            pdg,
            1
        );

        auto vtx = std::make_shared<HepMC3::GenVertex>(
            HepMC3::FourVector(0, 0, 0, 0)
        );

        vtx->add_particle_in(beam);
        vtx->add_particle_out(particle);

        evt.add_vertex(vtx);

        writer.write_event(evt);
    }

    writer.close();
}
