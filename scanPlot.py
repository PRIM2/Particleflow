#!/usr/bin/env python3
"""
Plot a 3D surface map from a particleflow geometry scan.

Axes:
  X = concreteHalfX [m]
  Y = frontPbHalfZ [m]
  Z = number of particles reaching a selected detector

This version draws a surface/map, so the result looks like a terrain:
  - 3D surface
  - colour map
  - contour projection on the bottom plane

Default input:
  results/geometry_scan/

Default detector:
  RPC

Examples:
  python plot_geometry_scan_surface.py
  python plot_geometry_scan_surface.py --detector ScintDet
  python plot_geometry_scan_surface.py --detector FrontPbTrap
  python plot_geometry_scan_surface.py --detector ConcreteWall
  python plot_geometry_scan_surface.py --no-show
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm


RUN_NAME_RE = re.compile(
    r"ev(?P<events>\d+)"
    r"_concreteHalfX_(?P<concrete>\d+p\d+)m"
    r"_frontPbHalfZ_(?P<pb>\d+p\d+)m"
)


def parse_value(tag: str) -> float:
    return float(tag.replace("p", "."))


def count_hepmc_particles(path: Path) -> int:
    """
    Count particles in a HepMC3 ASCII file.

    Your BaseSD exporter writes one HepMC particle line per recorded hit.
    In HepMC3 ASCII, those particle records normally start with 'P '.
    """
    count = 0

    with path.open("r", encoding="utf-8", errors="ignore") as file:
        for line in file:
            if line.startswith("P "):
                count += 1

    return count


def find_detector_file(run_dir: Path, detector: str) -> Path | None:
    detector_lower = detector.lower()

    candidates = sorted(run_dir.glob("*.hepmc"))

    matching = [
        path for path in candidates
        if detector_lower in path.name.lower()
    ]

    if matching:
        return matching[0]

    return None


def load_scan_points(results_root: Path, detector: str):
    points: list[tuple[float, float, int]] = []

    missing_detector_files = 0
    ignored_folders = 0

    for run_dir in sorted(results_root.iterdir()):
        if not run_dir.is_dir():
            continue

        match = RUN_NAME_RE.search(run_dir.name)

        if not match:
            ignored_folders += 1
            continue

        concrete = parse_value(match.group("concrete"))
        pb = parse_value(match.group("pb"))

        detector_file = find_detector_file(run_dir, detector)

        if detector_file is None:
            missing_detector_files += 1
            continue

        n_particles = count_hepmc_particles(detector_file)

        points.append((concrete, pb, n_particles))

    return points, missing_detector_files, ignored_folders


def build_grid(points: list[tuple[float, float, int]]):
    """
    Convert scattered scan points into a rectangular grid.

    This assumes the scan was generated as all combinations of:
      concreteHalfX × frontPbHalfZ

    Missing points are stored as NaN.
    """
    concrete_values = sorted({p[0] for p in points})
    pb_values = sorted({p[1] for p in points})

    x_index = {value: i for i, value in enumerate(concrete_values)}
    y_index = {value: i for i, value in enumerate(pb_values)}

    z_grid = np.full(
        (len(pb_values), len(concrete_values)),
        np.nan,
        dtype=float,
    )

    for concrete, pb, n_particles in points:
        z_grid[y_index[pb], x_index[concrete]] = n_particles

    x_grid, y_grid = np.meshgrid(concrete_values, pb_values)

    return x_grid, y_grid, z_grid


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Create a 3D surface map from a particleflow geometry scan."
    )

    parser.add_argument(
        "--results",
        default="results/geometry_scan",
        help="Folder containing the geometry scan result folders.",
    )

    parser.add_argument(
        "--detector",
        default="RPC",
        help="Detector name to count particles from, e.g. RPC, ScintDet, ConcreteWall, FrontPbTrap.",
    )

    parser.add_argument(
        "--output",
        default=None,
        help="Output image path. Default: results/geometry_scan/surface_<detector>.png",
    )

    parser.add_argument(
        "--no-show",
        action="store_true",
        help="Save the plot without opening an interactive window.",
    )

    parser.add_argument(
        "--elev",
        type=float,
        default=28.0,
        help="3D camera elevation angle.",
    )

    parser.add_argument(
        "--azim",
        type=float,
        default=-135.0,
        help="3D camera azimuth angle.",
    )

    args = parser.parse_args()

    results_root = Path(args.results)

    if not results_root.exists():
        raise FileNotFoundError(f"Results folder not found: {results_root}")

    points, missing_detector_files, ignored_folders = load_scan_points(
        results_root,
        args.detector,
    )

    if not points:
        raise RuntimeError(
            "No data found. Check --results, --detector and the folder/file names."
        )

    x_grid, y_grid, z_grid = build_grid(points)

    valid_points = np.count_nonzero(~np.isnan(z_grid))
    missing_grid_points = np.count_nonzero(np.isnan(z_grid))

    print(f"Loaded points: {len(points)}")
    print(f"Valid grid points: {valid_points}")
    print(f"Missing grid points: {missing_grid_points}")
    print(f"Missing detector files: {missing_detector_files}")
    print(f"Ignored folders: {ignored_folders}")

    if valid_points < 3:
        raise RuntimeError("Not enough points to draw a surface.")

    z_min = np.nanmin(z_grid)
    z_max = np.nanmax(z_grid)

    fig = plt.figure(figsize=(12, 8))
    ax = fig.add_subplot(111, projection="3d")

    surface = ax.plot_surface(
        x_grid,
        y_grid,
        z_grid,
        cmap=cm.viridis,
        linewidth=0.25,
        edgecolor="black",
        antialiased=True,
        alpha=0.95,
    )

    # Draw contour projection below the surface.
    z_offset = z_min - 0.08 * (z_max - z_min if z_max > z_min else 1.0)

    ax.contourf(
        x_grid,
        y_grid,
        z_grid,
        zdir="z",
        offset=z_offset,
        cmap=cm.viridis,
        alpha=0.75,
    )

    ax.contour(
        x_grid,
        y_grid,
        z_grid,
        zdir="z",
        offset=z_offset,
        colors="black",
        linewidths=0.4,
        alpha=0.55,
    )

    # Add original sampled points on top, useful to see the scan grid.
    ax.scatter(
        [p[0] for p in points],
        [p[1] for p in points],
        [p[2] for p in points],
        s=12,
        depthshade=True,
    )

    ax.set_xlabel("Concrete half-length X [m]")
    ax.set_ylabel("Front Pb half-length Z [m]")
    ax.set_zlabel(f"Particles reaching {args.detector}")
    ax.set_title(f"Geometry scan surface map: particles reaching {args.detector}")

    ax.set_zlim(z_offset, z_max)
    ax.view_init(elev=args.elev, azim=args.azim)

    fig.colorbar(
        surface,
        ax=ax,
        shrink=0.65,
        pad=0.1,
        label="Particle count",
    )

    output_path = (
        Path(args.output)
        if args.output is not None
        else results_root / f"surface_{args.detector}.png"
    )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    plt.tight_layout()
    plt.savefig(output_path, dpi=220)

    print(f"Saved plot: {output_path}")

    if not args.no_show:
        plt.show()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
