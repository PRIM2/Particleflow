# ParticleFlow

ParticleFlow is a C++17 simulation project that connects event generation with Geant4 detector simulation through HepMC3 event files.

It supports two event sources:

* a simple particle gun;
* Pythia8 generation from `.cmnd` files.

Generated events can be written to HepMC3, passed directly to Geant4, or read later from an existing HepMC3 file.

---

## Features

* C++17 simulation workflow.
* Particle gun event generation.
* Pythia8 event generation.
* HepMC3 input and output files.
* Geant4 detector simulation.
* Selectable Geant4 geometries.
* Optional Geant4 interactive UI.
* TOML-based run configuration.

---

## Requirements

ParticleFlow requires:

* CMake;
* a C++17 compatible compiler;
* Geant4;
* HepMC3;
* Pythia8.

The corresponding environment must be correctly configured before building the project. In particular, Geant4 datasets and runtime variables must be available.

Depending on the local installation, the following tools or paths may need to be visible to CMake:

```text
geant4-config
HepMC3-config
pythia8-config
```

---

## Repository structure

```text
.
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── CONFIG.md
├── config/
│   ├── cmndPythia/
│   ├── macrosG4/
│   └── runs/
├── include/
│   ├── config/
│   ├── generators/
│   └── geant4/
├── src/
│   ├── bin/
│   ├── config/
│   ├── generators/
│   └── geant4/
├── results/
├── notes/
└── build/
```

### Main directories

* `src/`: C++ source files.
* `include/`: project headers.
* `config/`: input configuration files, Pythia command files, and Geant4 macros.
* `config/runs/`: TOML run files.
* `config/cmndPythia/`: Pythia8 `.cmnd` files.
* `config/macrosG4/`: Geant4 visualization and export macros.
* `results/`: generated outputs such as HepMC3 files and plots.
* `notes/`: development notes and temporary material.
* `build/`: local CMake build directory.

`build/` is generated locally and can be safely removed and regenerated.

---

## Build

From the project root directory:

```bash
rm -rf build
cmake --preset default
cmake --build build -j8
```

For incremental builds:

```bash
cmake --build build -j8
```

The main executable is generated at:

```text
build/bin/particleflow
```

---

## Usage

Run ParticleFlow from the project root directory:

```bash
./build/bin/particleflow -c config/runs/full-example.toml
```

Show the help message:

```bash
./build/bin/particleflow -h
```

The executable accepts the following options:

```text
-c FILE   Run configuration file
-h        Show the help message
```

Paths passed with `-c` are interpreted relative to the project root directory.

---

## Configuration

Runs are configured through TOML files stored in `config/runs/`.

A complete example is provided at:

```text
config/runs/full-example.toml
```

The configuration format is documented separately in:

```text
CONFIG.md
```

---

## Workflow overview

ParticleFlow can be used in several ways depending on the selected run configuration:

1. Generate events with the particle gun.
2. Generate events with Pythia8.
3. Write generated events to HepMC3.
4. Read existing HepMC3 events.
5. Run events through Geant4.
6. Write the Geant4 output to a new HepMC3 file.

The selected workflow is controlled by the TOML run file. See `CONFIG.md` for the available options.

---

## Geant4 simulation

The Geant4 part of the project is implemented under:

```text
src/geant4/
include/geant4/
```

It includes:

* detector construction;
* action initialization;
* primary generation from HepMC3 input;
* sensitive detectors;
* selectable geometries;
* optional UI execution.

When the UI is disabled, the simulation runs directly in batch mode. When the UI is enabled, a Geant4 interactive session is started.

Visualization and export macros are stored in:

```text
config/macrosG4/
```

---

## Event generation

Event generation is implemented under:

```text
src/generators/
include/generators/
```

The project currently provides:

* `runPythia(...)`: reads a Pythia8 command file and writes generated events to HepMC3;
* `particleGun(...)`: generates fixed-particle HepMC3 events with momentum along the positive x-axis.

Pythia8 command files are stored in:

```text
config/cmndPythia/
```

---

## Output files

Output files are normally written under:

```text
results/
```

Typical outputs include:

* generated HepMC3 files;
* Geant4-processed HepMC3 files;
* plots or analysis files.

Generated output files are not required to build the project.

---

## Development notes

The current executable entry point is:

```text
src/bin/particleflow.cc
```

The main internal components are:

```text
src/config/       TOML parsing and configuration loading
src/generators/   Pythia8 and particle gun event generation
src/geant4/       Geant4 simulation code
```

Sensitive detectors are located in:

```text
src/geant4/detectorSDs/
include/geant4/detectorSDs/
```

Geometries are located in:

```text
src/geant4/geometries/
include/geant4/geometries/
```

---

## Status

ParticleFlow is under active development. Interfaces, geometries, configuration fields, and output conventions may change while the simulation workflow is being consolidated.
