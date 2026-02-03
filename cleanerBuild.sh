#!/usr/bin/env bash
set -u

BUILD_DIR="build"
OUT_LOG="geant4.log"
ERR_LOG="error.log"
GEANT4_DIR="/home/ferrangb/Desktop/entornos/Geant4/Binarios/lib64/cmake/Geant4"

read -r -p "Delete build/ and logs (${OUT_LOG}, ${ERR_LOG})? (yes/no): " input

case "${input,,}" in
  yes|y)
    rm -rf "$BUILD_DIR"
    rm -f "$ERR_LOG" "$OUT_LOG"
    echo "Deleted: ${BUILD_DIR}/, ${OUT_LOG}, ${ERR_LOG}"

    # Reconfigure and build from scratch
    mkdir -p "$BUILD_DIR"
    cmake -S . -B "$BUILD_DIR" -DGeant4_DIR="$GEANT4_DIR"
    cmake --build "$BUILD_DIR"
    ;;
  *)
    echo "Nothing deleted."
    ;;
esac
