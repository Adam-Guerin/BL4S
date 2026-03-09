#!/usr/bin/env python3
"""
BL4S-oriented multi-angle reconstruction from ArtAnalysisSimulation CSV outputs.

Input: CSV saved by /art/save (event-level section).
Output:
  - 2D areal density map
  - depth max-intensity projection
  - diagnostics plots
  - npy arrays for downstream analysis
"""

from __future__ import annotations

import argparse
import csv
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Sequence, Tuple

import matplotlib.pyplot as plt
import numpy as np


@dataclass
class ReconstructionConfig:
    nx: int = 48
    ny: int = 48
    nz: int = 20
    width_mm: float = 200.0
    height_mm: float = 200.0
    depth_mm: float = 2.11
    iterations: int = 40
    relaxation: float = 0.2
    weight_scattering_budget: float = 0.55
    weight_thickness: float = 0.30
    weight_energy: float = 0.15


def _safe_float(row: Dict[str, str], key: str, default: float = 0.0) -> float:
    value = row.get(key, "")
    if value is None or value == "":
        return default
    try:
        return float(value)
    except ValueError:
        return default


def _safe_int(row: Dict[str, str], key: str, default: int = 0) -> int:
    value = row.get(key, "")
    if value is None or value == "":
        return default
    try:
        return int(float(value))
    except ValueError:
        return default


def load_event_rows(csv_path: Path) -> List[Dict[str, str]]:
    """Read only event-level rows (ignore grid section)."""
    lines = csv_path.read_text(encoding="utf-8", errors="ignore").splitlines()

    header_idx = -1
    for i, line in enumerate(lines):
        if line.strip().startswith("event_id,"):
            header_idx = i
            break
    if header_idx < 0:
        raise ValueError("No event-level header found (expected line starting with 'event_id,').")

    event_lines: List[str] = [lines[header_idx]]
    for line in lines[header_idx + 1 :]:
        stripped = line.strip()
        if stripped == "" or stripped.startswith("#"):
            break
        event_lines.append(line)

    reader = csv.DictReader(event_lines)
    return list(reader)


def robust_scale(arr: np.ndarray) -> np.ndarray:
    if arr.size == 0:
        return arr
    q10, q90 = np.percentile(arr, [10, 90])
    scale = max(q90 - q10, 1e-9)
    return np.clip((arr - q10) / scale, 0.0, 1.0)


def make_voxel_path(
    x_mm: float,
    y_mm: float,
    proj_angle_y_rad: float,
    cfg: ReconstructionConfig,
) -> List[Tuple[int, int, int]]:
    """Construct a simple straight ray through z slices with y drift from projection angle."""
    half_w = cfg.width_mm / 2.0
    half_h = cfg.height_mm / 2.0
    z_centers = np.linspace(-cfg.depth_mm / 2.0, cfg.depth_mm / 2.0, cfg.nz)

    path: List[Tuple[int, int, int]] = []
    for iz, z_mm in enumerate(z_centers):
        y_at_z = y_mm + np.tan(proj_angle_y_rad) * z_mm
        if not (-half_w <= x_mm <= half_w and -half_h <= y_at_z <= half_h):
            continue
        ix = int(np.floor((x_mm + half_w) / cfg.width_mm * cfg.nx))
        iy = int(np.floor((y_at_z + half_h) / cfg.height_mm * cfg.ny))
        ix = min(max(ix, 0), cfg.nx - 1)
        iy = min(max(iy, 0), cfg.ny - 1)
        path.append((ix, iy, iz))

    return path


def reconstruct_volume(
    rows: Sequence[Dict[str, str]],
    cfg: ReconstructionConfig,
) -> Tuple[np.ndarray, np.ndarray]:
    valid_rows: List[Dict[str, str]] = []
    for row in rows:
        if _safe_int(row, "valid_tomography", 0) != 1:
            continue
        if "transmitted" in row and _safe_int(row, "transmitted", 0) != 1:
            continue
        valid_rows.append(row)

    if not valid_rows:
        raise ValueError("No valid tomography rows found.")

    theta_mrad = np.array([_safe_float(r, "theta_mrad", 0.0) for r in valid_rows], dtype=float)
    x_over_x0 = np.array([_safe_float(r, "x_over_X0_est", 0.0) for r in valid_rows], dtype=float)
    thickness_mm = np.array([_safe_float(r, "thickness_est_mm", 0.0) for r in valid_rows], dtype=float)
    delta_e_mev = np.array([_safe_float(r, "deltaE_MeV", 0.0) for r in valid_rows], dtype=float)

    # Prefer physically reconstructed observables when present in CSV.
    scattering_budget = np.where(x_over_x0 > 0.0, x_over_x0, np.maximum(theta_mrad, 0.0))
    thickness_proxy = np.where(thickness_mm > 0.0, thickness_mm, np.maximum(delta_e_mev, 0.0))

    budget_n = robust_scale(scattering_budget)
    thickness_n = robust_scale(thickness_proxy)
    de_n = robust_scale(delta_e_mev)
    y_meas = (
        cfg.weight_scattering_budget * budget_n
        + cfg.weight_thickness * thickness_n
        + cfg.weight_energy * de_n
    )

    paths: List[List[Tuple[int, int, int]]] = []
    for r in valid_rows:
        x_mm = _safe_float(r, "obj_x_mm", 0.0)
        y_mm = _safe_float(r, "obj_y_mm", 0.0)
        proj_mrad = _safe_float(r, "projection_angle_y_mrad", 0.0)
        proj_rad = proj_mrad * 1e-3
        path = make_voxel_path(x_mm, y_mm, proj_rad, cfg)
        if path:
            paths.append(path)
        else:
            paths.append([])

    vol = np.zeros((cfg.nx, cfg.ny, cfg.nz), dtype=float)
    eps = 1e-9
    for _ in range(cfg.iterations):
        for i, path in enumerate(paths):
            if not path:
                continue
            pred = np.mean([vol[ix, iy, iz] for ix, iy, iz in path])
            residual = y_meas[i] - pred
            update = cfg.relaxation * residual / max(len(path), 1)
            for ix, iy, iz in path:
                vol[ix, iy, iz] = max(0.0, vol[ix, iy, iz] + update)

        # Mild smoothing to stabilize noise without external deps.
        vol = 0.5 * vol + 0.5 * (
            np.roll(vol, 1, axis=0)
            + np.roll(vol, -1, axis=0)
            + np.roll(vol, 1, axis=1)
            + np.roll(vol, -1, axis=1)
        ) / 4.0
        vol = np.clip(vol, 0.0, None)

    y_pred = np.zeros_like(y_meas)
    for i, path in enumerate(paths):
        if path:
            y_pred[i] = np.mean([vol[ix, iy, iz] for ix, iy, iz in path])

    return vol, np.column_stack([y_meas, y_pred])


def save_outputs(
    out_dir: Path,
    volume: np.ndarray,
    obs_vs_pred: np.ndarray,
    cfg: ReconstructionConfig,
) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)

    areal_map = np.mean(volume, axis=2).T
    depth_mip = np.max(volume, axis=2).T

    np.save(out_dir / "reco_volume.npy", volume)
    np.save(out_dir / "reco_areal_map.npy", areal_map)
    np.save(out_dir / "reco_depth_mip.npy", depth_mip)

    plt.figure(figsize=(8, 6))
    plt.imshow(
        areal_map,
        origin="lower",
        cmap="inferno",
        extent=[-cfg.width_mm / 2, cfg.width_mm / 2, -cfg.height_mm / 2, cfg.height_mm / 2],
        aspect="equal",
    )
    plt.colorbar(label="Relative density (a.u.)")
    plt.xlabel("x [mm]")
    plt.ylabel("y [mm]")
    plt.title("2D Areal Density Reconstruction")
    plt.tight_layout()
    plt.savefig(out_dir / "reco_areal_map.png", dpi=220)
    plt.close()

    plt.figure(figsize=(8, 6))
    plt.imshow(
        depth_mip,
        origin="lower",
        cmap="viridis",
        extent=[-cfg.width_mm / 2, cfg.width_mm / 2, -cfg.height_mm / 2, cfg.height_mm / 2],
        aspect="equal",
    )
    plt.colorbar(label="Max voxel response (a.u.)")
    plt.xlabel("x [mm]")
    plt.ylabel("y [mm]")
    plt.title("Pseudo-3D Depth MIP")
    plt.tight_layout()
    plt.savefig(out_dir / "reco_depth_mip.png", dpi=220)
    plt.close()

    y_obs = obs_vs_pred[:, 0]
    y_pred = obs_vs_pred[:, 1]
    rms = float(np.sqrt(np.mean((y_obs - y_pred) ** 2)))
    corr = float(np.corrcoef(y_obs, y_pred)[0, 1]) if y_obs.size > 1 else 0.0

    plt.figure(figsize=(7, 6))
    plt.scatter(y_obs, y_pred, s=6, alpha=0.5)
    lim = [min(np.min(y_obs), np.min(y_pred)), max(np.max(y_obs), np.max(y_pred))]
    plt.plot(lim, lim, "r--", linewidth=1)
    plt.xlabel("Observed combined signal")
    plt.ylabel("Predicted combined signal")
    plt.title(f"Forward-model consistency (RMS={rms:.4f}, corr={corr:.3f})")
    plt.tight_layout()
    plt.savefig(out_dir / "reco_diagnostics.png", dpi=220)
    plt.close()

    summary = (
        f"nx={cfg.nx}, ny={cfg.ny}, nz={cfg.nz}\n"
        f"iterations={cfg.iterations}, relaxation={cfg.relaxation}\n"
        f"weight_scattering_budget={cfg.weight_scattering_budget}, "
        f"weight_thickness={cfg.weight_thickness}, weight_energy={cfg.weight_energy}\n"
        f"rms_residual={rms:.6f}\n"
        f"corr_obs_pred={corr:.6f}\n"
    )
    (out_dir / "reco_summary.txt").write_text(summary, encoding="utf-8")


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="BL4S multi-angle reconstruction")
    p.add_argument("--input", required=True, help="Path to /art/save CSV file")
    p.add_argument("--outdir", default="reconstruction_output", help="Output directory")
    p.add_argument("--nx", type=int, default=48)
    p.add_argument("--ny", type=int, default=48)
    p.add_argument("--nz", type=int, default=20)
    p.add_argument("--iterations", type=int, default=40)
    p.add_argument("--relaxation", type=float, default=0.2)
    p.add_argument("--weight-scattering-budget", type=float, default=0.55)
    p.add_argument("--weight-thickness", type=float, default=0.30)
    p.add_argument("--weight-energy", type=float, default=0.15)
    p.add_argument("--width-mm", type=float, default=200.0)
    p.add_argument("--height-mm", type=float, default=200.0)
    p.add_argument("--depth-mm", type=float, default=2.11)
    return p.parse_args()


def main() -> None:
    args = parse_args()
    cfg = ReconstructionConfig(
        nx=args.nx,
        ny=args.ny,
        nz=args.nz,
        width_mm=args.width_mm,
        height_mm=args.height_mm,
        depth_mm=args.depth_mm,
        iterations=args.iterations,
        relaxation=args.relaxation,
        weight_scattering_budget=args.weight_scattering_budget,
        weight_thickness=args.weight_thickness,
        weight_energy=args.weight_energy,
    )

    rows = load_event_rows(Path(args.input))
    volume, obs_vs_pred = reconstruct_volume(rows, cfg)
    save_outputs(Path(args.outdir), volume, obs_vs_pred, cfg)
    print(f"Reconstruction complete. Outputs written to: {args.outdir}")


if __name__ == "__main__":
    main()
