# pgun — Geant4 Particle Gun Simulation

Simulation of particles traversing a shielding system (lead + concrete) and an R-134a gas detector using [Geant4](https://geant4.org/).

---

## Quick Start

Clean build and compile:

```bash
rm -rf build
cmake -S . -B build
cmake --build build -j8
```

Interactive execution:

```bash
./build/bin/pgun
```

Batch execution with a macro:

```bash
./build/bin/pgun path/to/macro.mac
```

Simulation results are written to:

```
build/results/
```

---

## General Description

The program generates three primary particles per event:

- neutron
- negative muon
- positive kaon

Each particle has an energy of **20 GeV** and is fired in the **+Z direction** from `z = −19.8 m`.

The particles traverse the following geometry:

| Component | Dimensions | Center |
|---|---|---|
| Lead wall | 7 × 7 × 8 m | z = −5 m |
| Concrete wall | 7 × 7 × 7 m | z = +11.6 m |
| Gas detector layer 1 | 7 × 7 m, 4 cm thick | z = +18.8 m |
| Gas detector layer 2 | 7 × 7 m, 4 cm thick | z = +19.2 m |

The world volume is an air box of **8 × 8 × 40 m**.

The gas detector is instrumented with a Sensitive Detector (`GasSD`) that records simulation steps with energy deposition and writes one `.dat` file per event.

---

## Physics List

The simulation uses the Geant4 reference physics list:

```
FTFP_BERT
```

This includes:

- Fritiof high-energy hadronic model
- Bertini cascade for intermediate energies
- Standard electromagnetic physics

---

## Project Structure

```
pgun/
├── CMakeLists.txt              # CMake build configuration
├── CMakePresets.json           # CMake build presets (predefined configure/build settings)
├── src/
│   ├── pgun.cc                 # Entry point; sets RunManager, physics and UI
│   ├── DetectorConstruction.cc # Geometry definition
│   ├── PrimaryGeneratorAction.cc # Particle gun configuration
│   ├── ActionInitialization.cc # User actions registration
│   ├── GasSD.cc                # Sensitive detector implementation
│   └── Messages.cc             # Logging utilities
├── include/                    # Corresponding header files
├── macros/
│   ├── run.mac                 # Example batch macro
│   ├── vis.mac                 # Visualization macro
│   ├── init.mac                # Initialization macro
│   ├── zzlib.mac               # Useful Geant4 commands
│   └── zzlib_valid.mac         # Geometry validation macros
├── build/                      # CMake build directory
│   ├── bin/                    # Compiled executables
│   └── results/                # Simulation outputs
├── logs/                       # Optional execution logs
├── README.md
└── MAKE_VS_CMAKE.md
```

---

## Requirements

- Geant4 ≥ 11.x
- CMake ≥ 3.10
- C++17 compatible compiler (GCC or Clang)
- Geant4 must be correctly configured in your environment (datasets, libraries, etc.)

If CMake cannot find Geant4 automatically, configure it manually:

```bash
cmake -S . -B build -DGeant4_DIR=/path/to/Geant4/lib64/cmake/Geant4
```

---

## Build Workflow

### Clean Build

```bash
rm -rf build
cmake -S . -B build
cmake --build build -j8
```

### Incremental Build

If the project is already configured:

```bash
cmake --build build -j8
```

---

## Running the Simulation

### Interactive Mode

Run without arguments:

```bash
./build/bin/pgun
```

This starts the Geant4 interactive session (`Idle>`). Example commands:

```
/control/execute macros/init.mac
/control/execute macros/vis.mac
/run/beamOn 1
```

### Batch Mode

Run using a macro file:

```bash
./build/bin/pgun macros/run.mac
```

The macro controls initialization, visualization, and number of events. Example inside a macro:

```
/run/beamOn 10
```

---

## Output Data

For each event, the `GasSD` sensitive detector writes a file:

```
build/results/gas_event_<eventID>.dat
```

Example:

```
build/results/gas_event_0.dat
build/results/gas_event_1.dat
```

> This output path is independent of the directory from which the executable is launched.

### Output File Format

Each file contains two parts.

**Event Summary**

```
# eventID  Edep_gas_total[MeV]  Nsteps_in_gas  Ntracks_in_gas
0 0.1977342711 31 12
```

**Step-by-Step Data**

```
t0[ns],x0[mm],y0[mm],z0[mm],x1[mm],y1[mm],z1[mm],Edep[MeV],event_id,track_id,pdg,volume_name,volume_copyNo
```

Each row corresponds to a simulation step inside the gas volume with energy deposition > 0.

| Field | Description |
|---|---|
| `t0` | Global time at the beginning of the step (ns) |
| `x0, y0, z0` | Position at the beginning of the step (mm) |
| `x1, y1, z1` | Position at the end of the step (mm) |
| `Edep` | Energy deposited during the step (MeV) |
| `event_id` | Event identifier |
| `track_id` | Particle track identifier |
| `pdg` | Particle PDG code |
| `volume_name` | Detector volume name |
| `volume_copyNo` | Copy number of the detector volume |

---

## Modifying the Simulation

### Changing the Particle Gun

Edit `src/PrimaryGeneratorAction.cc`. Parameters that can be modified:

**Energy:**
```cpp
fGun->SetParticleEnergy(20.*GeV);
```

**Direction:**
```cpp
fGun->SetParticleMomentumDirection({0.,0.,1.});
```

**Initial position:**
```cpp
fGun->SetParticlePosition({0.,0.,-19.8*m});
```

**Particle type:**
```cpp
particleTable->FindParticle("mu-")
particleTable->FindParticle("proton")
particleTable->FindParticle("gamma")
```

### Changing the Geometry

Edit `src/DetectorConstruction.cc`. Geometry components include:

- world volume
- lead wall
- concrete wall
- gas detector layers

Materials are defined using the Geant4 NIST material database. The R-134a gas mixture (C₂H₂F₄) is defined explicitly from its elemental composition.

### Changing the Number of Events

Edit the macro file `macros/run.mac`:

```
/run/beamOn 100
```

You can also create your own macros to define different run configurations.

---

## Notes

The project uses out-of-source builds, meaning all compilation artifacts remain inside `build/`. This keeps the source tree clean and allows rebuilding the entire project simply by deleting the `build/` directory.