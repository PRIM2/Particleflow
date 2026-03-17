cmake_minimum_required(VERSION 3.10)

project(pgun)

# Ejecutables en build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# SET UP GEANT4
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})


# ******* SET UP PYTHIA8 ********
set(PYTHIA8_CFG "" CACHE FILEPATH "Path to pythia8-config")

if(NOT PYTHIA8_CFG)
  message(FATAL_ERROR "PYTHIA8_CFG no definido. Pásalo desde CMakePresets.json o con -DPYTHIA8_CFG=...")
endif()

execute_process(
  COMMAND ${PYTHIA8_CFG} --prefix
  OUTPUT_VARIABLE PYTHIA8_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# *******************************


set(PGUN_SOURCES
  src/bin/pgun.cc
  src/ActionInitialization.cc
  src/DetectorConstruction.cc
  src/PrimaryGeneratorAction.cc
  src/GasSD.cc
  src/Collider.cc
)

add_executable(pgun ${PGUN_SOURCES})

target_include_directories(pgun PRIVATE
  ${PROJECT_SOURCE_DIR}/include #este es el mio
  ${PYTHIA8_PREFIX}/include
  ${HEPMC3_PREFIX}/include
)

target_link_directories(pgun PRIVATE
  ${PYTHIA8_PREFIX}/lib
  ${HEPMC3_PREFIX}/lib
)

target_link_libraries(pgun PRIVATE
  ${Geant4_LIBRARIES}
  pythia8
  HepMC3
)

target_compile_definitions(pgun PRIVATE BUILD_DIR="${CMAKE_BINARY_DIR}")
target_compile_definitions(pgun PRIVATE PROJECT_DIR="${CMAKE_SOURCE_DIR}")