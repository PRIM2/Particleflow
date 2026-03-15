cmake_minimum_required(VERSION 3.10)

project(pgun)

# Ejecutables en build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

set(PGUN_SOURCES
  src/main.cc
  src/ActionInitialization.cc
  src/Messages.cc
  src/DetectorConstruction.cc
  src/PrimaryGeneratorAction.cc
  src/GasSD.cc
)

add_executable(pgun ${PGUN_SOURCES})

target_include_directories(pgun PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(pgun ${Geant4_LIBRARIES})

target_compile_definitions(pgun PRIVATE BUILD_DIR="${CMAKE_BINARY_DIR}")