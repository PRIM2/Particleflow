// Generators.cc
#include "generators/Generators.hh"

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

void runPythia(
    int nEvents,
    const config::PythiaConfig& cfg,
    const std::string& outputFile
) {
    Pythia8::Pythia pythia;
    HepMC3::Pythia8ToHepMC3 toHepMC;

    pythia.readFile(cfg.cmndFile);

    std::filesystem::create_directories(
        std::filesystem::path(outputFile).parent_path()
    );

    HepMC3::WriterAscii writer(outputFile);

    if (!pythia.init()) {
        throw std::runtime_error("PYTHIA initialization failed.");
    }

    for (int i = 0; i < nEvents; i++) {
        if (!pythia.next()) {
            continue;
        }

        HepMC3::GenEvent evt;
        toHepMC.fill_next_event(pythia, &evt);
        evt.set_event_number(i);

        /*
        // To change PYTHIA direction in the future.
        for (auto& p : evt.particles()) {
            auto m = p->momentum();

            p->set_momentum(HepMC3::FourVector(
                m.px(),
                m.py(),
                m.pz(),
                m.e()
            ));
        }

        for (auto& v : evt.vertices()) {
            auto pos = v->position();

            v->set_position(HepMC3::FourVector(
                pos.x(),
                pos.y(),
                pos.z(),
                pos.t()
            ));
        }
        */

        writer.write_event(evt);
    }

    writer.close();
}


void particleGun(
    int nEvents,
    const config::PgunConfig& cfg,
    const std::string& outputFile
) {
    if (cfg.energy < cfg.mass) {
        throw std::runtime_error(
            "Particle energy must be greater than or equal to mass."
        );
    }

    std::filesystem::create_directories(
        std::filesystem::path(outputFile).parent_path()
    );

    HepMC3::WriterAscii writer(outputFile);

    const double px = std::sqrt(cfg.energy * cfg.energy - cfg.mass * cfg.mass);

    for (int i = 0; i < nEvents; i++) {
        HepMC3::GenEvent evt(HepMC3::Units::GEV, HepMC3::Units::MM);

        evt.set_event_number(i);

        auto beam = std::make_shared<HepMC3::GenParticle>(
            HepMC3::FourVector(px, 0, 0, cfg.energy),
            cfg.pdg,
            4
        );

        auto particle = std::make_shared<HepMC3::GenParticle>(
            HepMC3::FourVector(px, 0, 0, cfg.energy),
            cfg.pdg,
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