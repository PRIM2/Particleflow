#!/usr/bin/env bash
set -u

BUILD_DIR="build"
BIN="./${BUILD_DIR}/pgun"

mkdir -p logs

cmake --build "$BUILD_DIR" 2> logs/build_ui.err
build_status=$?

if [ "$build_status" -ne 0 ]; then
  echo "Build failed. Mira logs/build_ui.err"
  cat logs/build_ui.err
  exit "$build_status"
fi

if [ ! -x "$BIN" ]; then
  echo "Not executed: $BIN does not exist or is not executable"
  exit 1
fi

# IMPORTANTÍSIMO: sin redirecciones para ver el Idle> y poder escribir comandos
script -q -c "./build/pgun" logs/geant4-ui.log
