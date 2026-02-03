#!/usr/bin/env bash
set -u

###############################################################################
# Geant4 runner (batch mode)
#
# What this script does:
#  1) Builds the project using CMake in the build/ directory.
#  2) Runs the Geant4 executable in batch mode, executing a macro file passed
#     as the FIRST argument to this script.
#  3) Writes stdout/stderr to logs/ for easy debugging.
#
# Usage:
#   ./run.sh path/to/macro.mac
#
# Examples:
#   ./run.sh macros/run.mac
#   ./run.sh macros/vis.mac
#
# Notes:
#  - Batch mode means the program will execute the macro and then exit.
#  - If you want an interactive Idle> prompt, do NOT use this script; run the
#    run_ui.sh or directly execute the binary.
###############################################################################

BUILD_DIR="build"
BIN="./${BUILD_DIR}/pgun"

# First argument = macro to execute
MACRO="${1-}"

OUT_LOG="logs/geant4-batch.log"
ERR_LOG="logs/error-batch.log"

mkdir -p logs
: > "$OUT_LOG"
: > "$ERR_LOG"

# Build
cmake --build "$BUILD_DIR" >>"$OUT_LOG" 2>>"$ERR_LOG"
build_status=$?

if [ "$build_status" -ne 0 ]; then
  echo "Not executed: build failed"
  echo
  cat "$ERR_LOG"
  exit "$build_status"
fi

# Check binary
if [ ! -x "$BIN" ]; then
  echo "Not executed: $BIN does not exist or is not executable" | tee -a "$ERR_LOG"
  exit 1
fi

# Check macro argument
if [ -z "$MACRO" ]; then
  echo "Not executed: missing macro argument." | tee -a "$ERR_LOG"
  echo "Usage: ./run.sh path/to/macro.mac" | tee -a "$ERR_LOG"
  exit 2
fi

# Check macro file exists
if [ ! -f "$MACRO" ]; then
  echo "Not executed: macro file '$MACRO' not found" | tee -a "$ERR_LOG"
  exit 1
fi

# Run binary + macro (batch mode)
/usr/bin/env "$BIN" "$MACRO" >>"$OUT_LOG" 2>>"$ERR_LOG"

echo "---- STDOUT ($OUT_LOG) ----"
cat "$OUT_LOG"

if [ -s "$ERR_LOG" ]; then
  echo
  echo "---- STDERR ($ERR_LOG) ----"
  cat "$ERR_LOG"
fi
