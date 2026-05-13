# Quick guide to the TOML configuration file

- Energies are in GeV.
- Distances are in meters (m).
- Angles are in degrees.
- Paths must be relative to the project root directory, not to the build directory.
- This file controls three main parts: event generation, Geant4 execution, and geometric filtering.

---

## `[Run]`

- General run configuration.

  - Number of events to generate or process.  
    `[int]`  
    `nEvents = 100`

  - Main execution mode. It defines which parts of the program are run.  
    `[string]`  
    `mode = "pgun-and-run"`  
      - `"pythia-and-run"`: generate events with Pythia and then simulate them with Geant4.  
      - `"pgun-and-run"`: generate events with the particle gun and then simulate them with Geant4.  
      - `"run-from-hepmc"`: do not generate new events; read an existing HepMC file and simulate it with Geant4.  
      - `"pgun-only"`: only generate events with the particle gun and save them to HepMC.  
      - `"pythia-only"`: only generate events with Pythia and save them to HepMC.

  - Output HepMC file for the events generated before Geant4.  
    `[string]`  
    `hepmcOutput = "resultss/ALOHA.hepmc"`

---

## `[Pythia]`

- Pythia generator configuration.

  - `.cmnd` file containing the Pythia parameters. It is only used in modes that include Pythia.  
    `[string]`  
    `cmndFile = "config/cmndPythia/test.cmnd"`

---

## `[Pgun]`

- Particle gun configuration.
- Only used in modes that include `pgun`.

  - Energy of the generated particle.  
    `[double]`  
    `energy = 100.0`

  - PDG code of the generated particle.  
    `[int]`  
    `pdg = 2212`  
      - `2212`: proton.  
      - `11`: electron.  
      - `-11`: positron.  
      - `13`: negative muon.  
      - `-13`: positive muon.  
      - `22`: photon.

  - Mass of the generated particle. It must be consistent with the selected PDG code.  
    `[double]`  
    `mass = 0.938272`

---

## `[Geant4]`

- Geant4 simulation configuration.

  - Input HepMC file for Geant4.  
    `[string]`  
    `input = "resultss/ALOHA.hepmc"`  
      - In `pgun-and-run` or `pythia-and-run`, this should normally match `Run.hepmcOutput`.  
      - In `run-from-hepmc`, this must point to an already existing HepMC file.

  - Output HepMC file after the Geant4 simulation.  
    `[string]`  
    `output = "resultss/Callate2.0.hepmc"`

  - Enable or disable the Geant4 visual/interactive interface.  
    `[bool]`  
    `ui = false`  
      - `false`: automatic execution, useful for production or quick tests.  
      - `true`: open the Geant4 interface, useful for visualising geometry and events.

  - Geometry to be built in Geant4.  
    `[string]`  
    `geometry = "SimpleWall"`  
      - `"SimpleWall"`: simplified geometry with a basic wall/detector setup.  
      - `"Realistic"`: more realistic geometry of the system.  
      - Any other value will produce an unknown geometry error.

---

## `[Geant4.AngularFilter]`

- Angular filter applied to particles before or during the simulation.
- It keeps only particles compatible with a direction or geometric region of interest.

  - Enable or disable the angular filter.  
    `[bool]`  
    `enabled = true`  
      - `true`: apply the filter.  
      - `false`: do not filter particles by angle.

  - X coordinate of the target point used by the filter.  
    `[double]`  
    `targetX = 29.74142`

  - Y coordinate of the target point used by the filter.  
    `[double]`  
    `targetY = -4.26804`

  - Z coordinate of the target point used by the filter.  
    `[double]`  
    `targetZ = 9.33062`

  - Maximum allowed angular aperture with respect to the target.  
    `[double]`  
    `theta = 50.0`

---

## Complete example

```toml
[Run]
nEvents = 100
mode = "pgun-and-run"
hepmcOutput = "results/test_PGUN.hepmc"

[Pythia]
cmndFile = "config/cmndPythia/test.cmnd"

[Pgun]
energy = 100.0
pdg = 2212
mass = 0.938272

[Geant4]
input = "results/test_PGUN.hepmc"
output = "results/test_PARTICLEFLOW.hepmc"
ui = false
geometry = "SimpleWall"

[Geant4.AngularFilter]
enabled = true
targetX = 29.74142
targetY = -4.26804
targetZ = 9.33062
theta = 50.0
```
