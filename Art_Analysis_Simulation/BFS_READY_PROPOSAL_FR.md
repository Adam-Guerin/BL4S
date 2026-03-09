# Particle Beam Tomography Using Relativistic Electrons for Non-Invasive Material Identification (BFS-ready)

## 1. Scientific Question
Can we reconstruct the internal structure of a layered object by combining:
- the multiple Coulomb scattering of relativistic electrons, and
- their energy loss after crossing the object?

## 2. Hypothesis
- The RMS scattering angle \(\theta_0\) increases with traversed material budget \(x/X_0\).
- The energy loss \(\Delta E\) increases with integrated stopping power and thickness.
- Combining \(\theta\) and \(\Delta E\) improves separation between materials with similar thickness but different composition.

## 3. Beamline-Compatible Experimental Setup (T9 / BL4S style)
### 3.1 Beam and trigger
- Relativistic electron beam (nominally in the GeV range).
- Upstream trigger scintillators for event timing and clean beam selection.

### 3.2 Tracking
- 2 upstream tracking planes (incoming direction).
- 2 downstream tracking planes (outgoing direction).
- Track fit before/after target gives event-by-event scattering angle.

### 3.3 Target (mock painting / layered phantom)
Use calibration targets with known ground truth:
1. Single-layer references (plastic, Al, graphite, Pb thin foil).
2. Two-layer combinations.
3. Blind multilayer target (known by supervisors, hidden during analysis).

### 3.4 Calorimetry
- One downstream electromagnetic calorimeter to estimate final electron energy \(E_f\).
- Initial energy \(E_i\) from beam setting/calibration runs.
- Compute \(\Delta E = E_i - E_f\).

## 4. Core Physics Model (used in reconstruction)
### 4.1 Multiple Coulomb scattering (Highland)
\[
\theta_0 \approx \frac{13.6\,\text{MeV}}{\beta p c}\, z\,\sqrt{\frac{x}{X_0}}\left[1+0.038\ln\left(\frac{x}{X_0}\right)\right]
\]
For electrons: \(z=1\). This links angular broadening to radiation-length budget.

### 4.2 Energy loss
For electrons, use Geant4 truth/reconstruction tables that include:
- collisional losses (ionization/excitation), and
- radiative losses (bremsstrahlung), especially relevant in high-Z materials.

Operational observable:
\[
\Delta E = E_i - E_f
\]

### 4.3 Optional transmission observable
Use counting, not power:
\[
T = \frac{N_{\text{transmitted}}}{N_{\text{incident}}}
\]
This is safer than deriving intensity from calorimetric power for event-based particle counting.

## 5. Reconstruction Strategy
### 5.1 Event-level observables
For each accepted event:
- incoming unit direction \(\hat{u}_{in}\),
- outgoing unit direction \(\hat{u}_{out}\),
- scattering angle \(\theta = \arccos(\hat{u}_{in}\cdot\hat{u}_{out})\),
- energy loss \(\Delta E\),
- impact point on target plane \((x,y)\).

### 5.2 Pixelized inversion
Discretize target in pixels/voxels.
For projection/path \(j\):
\[
y^{(\theta)}_j \approx \sum_i L_{ji}\,\lambda_i, \quad \lambda_i \propto 1/X_{0,i}
\]
\[
y^{(E)}_j \approx \sum_i L_{ji}\,\kappa_i
\]
where \(L_{ji}\) is path length in voxel \(i\), \(\lambda_i\) is scattering-density proxy, and \(\kappa_i\) is stopping-power proxy.

Estimate maps by constrained regularized fit (non-negative least squares / MAP).

### 5.3 Acquisition geometry
- Translation scan (x-y grid) and/or small-angle target rotation.
- Collect multiple projections to reduce ambiguity.

## 6. Concrete BL4S Run Plan
### Phase A: Detector and beam calibration
- No target + reference slabs.
- Measure angular resolution floor and calorimeter response.

### Phase B: Material discrimination
- Run known single/two-layer samples.
- Build calibration curves: \(\sigma_\theta\) vs \(x/X_0\), \(\Delta E\) vs thickness.

### Phase C: Blind target reconstruction
- Acquire multi-position / multi-angle data.
- Reconstruct maps and compare to hidden truth at end.

## 7. Success Criteria
Primary:
- Recover layer order correctly in blind sample.
- Distinguish high-Z vs low-Z regions with statistical significance.

Secondary:
- Quantify spatial resolution (mm scale) and thickness resolution.
- Report agreement between measured scattering width and Highland expectation.

## 8. Risks and Mitigation
- Limited beam time: prioritize calibration + one blind target.
- Multiple scattering in detectors: include detector material in Geant4 model and unfolding.
- Electron radiative tails: use robust estimators (median/trimmed mean) and quality cuts.
- Systematics: perform empty-target and reference-target control runs.

## 9. Direct Answers to Jury-Style Questions
1. Trajectories: approximately straight between interactions; inside matter, piecewise-deflected random walk (multiple scattering), not parabolic in this setup.
2. Detectors per step: trigger scintillators -> 2 upstream trackers -> layered object -> 2 downstream trackers -> EM calorimeter.
3. Key formulas: Highland for scattering; event-based \(\Delta E\) with electron stopping/radiative losses; optional transmission \(T=N_{trans}/N_{inc}\).

## 10. Why this is BFS-strong
- Clear measurable observables (\(\theta, \Delta E, T\)).
- Realistic detector chain used in beam tests.
- Incremental plan (calibration -> validation -> blind reconstruction).
- Deliverable at beamtime end: quantitative reconstruction with uncertainty.
