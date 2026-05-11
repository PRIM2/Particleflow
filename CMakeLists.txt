cmake_minimum_required(VERSION 3.10)

project(particleflow)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

set(PYTHIA8_CFG "" CACHE FILEPATH "Path to pythia8-config")
if(NOT PYTHIA8_CFG)
  message(FATAL_ERROR "PYTHIA8_CFG no definido. Pásalo desde CMakePresets.json o con -DPYTHIA8_CFG=...")
endif()

execute_process(
  COMMAND ${PYTHIA8_CFG} --prefix
  OUTPUT_VARIABLE PYTHIA8_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

file(GLOB GEOMETRY_SOURCES CONFIGURE_DEPENDS
  src/geant4/geometries/*.cc
)

file(GLOB DETECTORSD_SOURCES CONFIGURE_DEPENDS
  src/geant4/detectorSDs/*.cc
)

set(SHARED_SOURCES
  src/geant4/ActionInitialization.cc
  src/geant4/DetectorConstruction.cc
  src/geant4/PrimaryGeneratorAction.cc
  src/geant4/G4Simulator.cc
  src/geant4/StackingAction.cc
  src/generators/Generators.cc
  src/config/ConfigParser.cc
  ${GEOMETRY_SOURCES}
  ${DETECTORSD_SOURCES}
)

function(configure_target TARGET_NAME)
  target_include_directories(${TARGET_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PYTHIA8_PREFIX}/include
    ${HEPMC3_PREFIX}/include
  )

  target_link_directories(${TARGET_NAME} PRIVATE
    ${PYTHIA8_PREFIX}/lib
    ${HEPMC3_PREFIX}/lib
  )

  target_link_libraries(${TARGET_NAME} PRIVATE
    ${Geant4_LIBRARIES}
    pythia8
    HepMC3
  )

  target_compile_definitions(${TARGET_NAME} PRIVATE
    BUILD_DIR="${CMAKE_BINARY_DIR}"
    PROJECT_DIR="${CMAKE_SOURCE_DIR}"
  )

  get_target_property(GEANT4_LIB_DIR Geant4::G4tracking LOCATION)
  cmake_path(GET GEANT4_LIB_DIR PARENT_PATH GEANT4_LIB_DIR)

  set_target_properties(${TARGET_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "${GEANT4_LIB_DIR};${PYTHIA8_PREFIX}/lib;${HEPMC3_PREFIX}/lib"
  )

  target_link_options(${TARGET_NAME} PRIVATE "-Wl,--disable-new-dtags")
endfunction()

add_executable(particleflow
  src/bin/particleflow.cc
  ${SHARED_SOURCES}
)

configure_target(particleflow)