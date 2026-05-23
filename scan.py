#!/usr/bin/env python3
"""
Run a geometry scan for particleflow.

It creates one TOML file per geometry point and runs the binary once per point.

Scan:
  concreteHalfX: from 1.75 m down to 0.00 m in steps of 0.20 m
  frontPbHalfZ:  from 1.25 m down to 0.00 m in steps of 0.02 m

Each simulation is saved in a different folder under results/.
Output filenames include the number of events and both geometry values.

Run from the project root, for example:
  python3 scripts/run_geometry_scan.py

Or specify paths:
  python3 scripts/run_geometry_scan.py --binary build/bin/particleflow --input results/hard_100000ev_14tev_PYTHIA.hepmc
"""

from __future__ import annotations

import argparse
import os
import subprocess
from pathlib import Path


N_EVENTS = 1000

CONCRETE_START = 1.75
CONCRETE_STEP = 0.20

FRONT_PB_START = 1.25
FRONT_PB_STEP = 0.02

def descending_values(start: float, step: float) -> list[float]:
    """
    Generate values from start down to the last positive value.
    It does not include 0.00, because Geant4 solids cannot have zero thickness.
    """
    values: list[float] = []
    value = start

    while value > 0:
        rounded = round(value, 2)

        if rounded > 0.0:
            values.append(rounded)

        value -= step

    return values

def value_tag(value: float) -> str:
    """
    Convert a float to a filesystem-safe tag.

    1.75 -> 1p75
    0.33 -> 0p33
    """
    return f"{value:.2f}".replace(".", "p")


def make_config_text(
    *,
    n_events: int,
    input_hepmc: str,
    output_hepmc: str,
    concrete_half_x: float,
    front_pb_half_z: float,
    geometry: str,
    ui: bool,
) -> str:
    ui_text = "true" if ui else "false"

    return f"""\
[Run]
nEvents = {n_events}
mode = "run-from-hepmc"
hepmcOutput = "{input_hepmc}"

[Pythia]
cmndFile = "config/cmndPythia/test.cmnd"

[Pgun]
energy = 100.0
pdg = 2212
mass = 0.938272

[Geant4]
input = "{input_hepmc}"
output = "{output_hepmc}"
ui = {ui_text}
geometry = "{geometry}"

[Geant4.AngularFilter]
enabled = false
targetX = 0.0
targetY = 0.0
targetZ = 0.0
theta = 50.0

[Geant4.Geometry.RealisticMap]
concreteHalfX = {concrete_half_x:.2f}
frontPbHalfZ = {front_pb_half_z:.2f}
"""



def run_one(command: list[str], project_dir: Path) -> int:
    """
    Run one simulation.

    stdout and stderr are inherited by the terminal, so you can still see
    Geant4/config errors directly.
    """
    completed = subprocess.run(command, cwd=project_dir)
    return completed.returncode


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Run a concrete/Pb geometry scan with particleflow."
    )

    parser.add_argument(
        "--binary",
        default="build/bin/particleflow",
        help="Path to the particleflow binary from the project root.",
    )

    parser.add_argument(
        "--input",
        default="results/hard_100000ev_14tev_PYTHIA.hepmc",
        help="Input HepMC file, relative to the project root.",
    )

    parser.add_argument(
        "--geometry",
        default="RealisticMap",
        help="Geometry name passed to [Geant4].geometry.",
    )

    parser.add_argument(
        "--ui",
        action="store_true",
        help="Enable Geant4 UI/visualization. Usually keep this disabled for scans.",
    )

    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Only create TOML files and print commands; do not run the binary.",
    )

    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=os.cpu_count() or 1,
        help="Number of simulations to run in parallel. Default: number of CPU cores.",
    )

    args = parser.parse_args()

    project_dir = Path.cwd()

    binary_path = project_dir / args.binary
    if not binary_path.exists():
        raise FileNotFoundError(f"Binary not found: {binary_path}")

    input_path = project_dir / args.input
    if not input_path.exists():
        raise FileNotFoundError(f"Input HepMC file not found: {input_path}")

    concrete_values = descending_values(CONCRETE_START, CONCRETE_STEP)
    front_pb_values = descending_values(FRONT_PB_START, FRONT_PB_STEP)

    total = len(concrete_values) * len(front_pb_values)

    print(f"Concrete values: {len(concrete_values)}")
    print(f"Front Pb values: {len(front_pb_values)}")
    print(f"Total simulations: {total}")

    config_dir = project_dir / "config" / "runs" / "geometry_scan"
    config_dir.mkdir(parents=True, exist_ok=True)

    results_root = project_dir / "results" / "geometry_scan"
    results_root.mkdir(parents=True, exist_ok=True)

    jobs = max(1, min(args.jobs, total))
    print(f"Parallel jobs: {jobs}")

    tasks: list[tuple[int, list[str], str]] = []
    counter = 0

    for concrete in concrete_values:
        for front_pb in front_pb_values:
            counter += 1

            concrete_tag = value_tag(concrete)
            front_pb_tag = value_tag(front_pb)

            run_name = (
                f"ev{N_EVENTS}"
                f"_concreteHalfX_{concrete_tag}m"
                f"_frontPbHalfZ_{front_pb_tag}m"
            )

            run_results_dir = results_root / run_name
            run_results_dir.mkdir(parents=True, exist_ok=True)

            output_hepmc = (
                f"results/geometry_scan/{run_name}/"
                f"{run_name}_PARTICLEFLOW.hepmc"
            )

            config_rel = f"config/runs/geometry_scan/{run_name}.toml"
            config_path = project_dir / config_rel

            config_text = make_config_text(
                n_events=N_EVENTS,
                input_hepmc=args.input,
                output_hepmc=output_hepmc,
                concrete_half_x=concrete,
                front_pb_half_z=front_pb,
                geometry=args.geometry,
                ui=args.ui,
            )

            config_path.write_text(config_text, encoding="utf-8")

            command = [str(binary_path), "-c", config_rel]

            description = (
                f"concreteHalfX={concrete:.2f} m, "
                f"frontPbHalfZ={front_pb:.2f} m"
            )

            tasks.append((counter, command, description))

    if args.dry_run:
        for index, command, description in tasks:
            print(f"[{index:04d}/{total}] {description}")
            print("  " + " ".join(command))
        print("Dry run finished.")
        return 0

    # Multiprocessing is used here because each simulation is an independent
    # external process. This is safer than Python threads for this use case.
    from concurrent.futures import ThreadPoolExecutor, as_completed

    failures: list[tuple[int, int, str]] = []

    with ThreadPoolExecutor(max_workers=jobs) as executor:
        future_to_task = {
            executor.submit(run_one, command, project_dir): (index, description)
            for index, command, description in tasks
        }

        finished = 0

        for future in as_completed(future_to_task):
            index, description = future_to_task[future]
            finished += 1

            try:
                return_code = future.result()
            except Exception as exc:
                failures.append((index, -1, f"{description} | exception: {exc}"))
                print(f"[{finished:04d}/{total}] FAILED task {index:04d}: {description}")
                continue

            if return_code != 0:
                failures.append((index, return_code, description))
                print(
                    f"[{finished:04d}/{total}] FAILED task {index:04d} "
                    f"return_code={return_code}: {description}"
                )
            else:
                print(f"[{finished:04d}/{total}] OK task {index:04d}: {description}")

    if failures:
        print("\nSome simulations failed:")
        for index, return_code, description in failures:
            print(f"  task {index:04d}, return_code={return_code}: {description}")
        return 1

    print("Scan finished.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
