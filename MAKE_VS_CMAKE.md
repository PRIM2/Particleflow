
# Quick Comparison: Make vs CMake

## Basic Idea

| Tool | Function |
|---|---|
| **make** | Executes compilation rules defined in a `Makefile`. |
| **CMake** | Generates the build system (Makefiles, Ninja, etc.) from `CMakeLists.txt`. |

Typical chain:

CMake → generates → Makefile / Ninja → compiles with → gcc / g++

---

# Command Equivalences

| Action | Make | CMake |
|---|---|---|
| Build project | `make` | `cmake --build build` |
| Parallel build | `make -j8` | `cmake --build build -j8` |
| Build a specific target | `make pgun` | `cmake --build build --target pgun` |
| Clean build artifacts | `make clean` | `cmake --build build --target clean` |
| Show available targets | `make help` | `cmake --build build --target help` |

---

# Project Configuration

Before building with CMake, you must **configure the project**.

| Action | Command |
|---|---|
| Configure project | `cmake -S . -B build` |
| Reconfigure | `cmake -S . -B build` |

This generates the build system inside `build/`.

---

# Build Types

| Type | Command |
|---|---|
| Debug | `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug` |
| Release | `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` |

---

# Complete Cleanup (most common in CMake)

In CMake projects it is common to remove the entire build directory:

| REBUILD CMAKE |
|---|
| rm -rf build |
| cmake -S . -B build |
| cmake --build build |

This is called an **out-of-source build**.

---

# Typical Workflow

First build:

||
|-|
cmake -S . -B build  
cmake --build build  

Subsequent builds:

||
|-|
cmake --build build  

If something breaks:

||
|-|
rm -rf build  
cmake -S . -B build  
cmake --build build  

---

# Summary

| Concept | Make | CMake |
|---|---|---|
| Describes the build | `Makefile` | `CMakeLists.txt` |
| Generates build rules | manual | automatic |
| Executes compilation | yes | no |
| Actual compiler | colspan=2 `gcc / g++ / clang` |