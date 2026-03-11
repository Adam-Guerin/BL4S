# Art Analysis Simulation (Geant4)

Simulation Geant4 d'une tomographie non invasive par électrons relativistes, orientée BL4S:
- mesure de l'angle de diffusion multiple (trackers amont/aval),
- mesure de perte d'énergie (calorimètres),
- estimation physique de `x/X0`, `x`, `X0`, transmission et `mu`,
- reconstruction multi-angle.

## 1. Ce que fait le projet

Le setup simulé est:
`TriggerScintillator -> TrackerUp1/Up2 -> objet multicouche -> TrackerDown1/Down2 -> CalorimeterIn/Out`

L'objet contient des couches type peinture/restauration:
`Varnish, ModernPaint, Vermilion, LapisLazuli, LeadWhite, Malachite, LeadTinYellow, Gesso, Canvas`.
Des inclusions latérales cachées (patch/stripe/window) sont aussi placées dans certaines couches pour créer une vraie structure 2D non uniforme.

Le faisceau primaire est un faisceau d'électrons (`e-`) avec énergie configurable (2 GeV dans les macros BL4S).

## 2. Prérequis

- CMake >= 3.12
- Compilateur C++17
- Geant4 installé et détectable par CMake (`find_package(Geant4 REQUIRED)`)
- Dataset Geant4 `G4ENSDFSTATEDATA` correctement configuré

Au runtime, si `G4ENSDFSTATEDATA/ENSDFSTATE.dat` est absent, l'exécutable s'arrête avec code `2`.

## 3. Build

Depuis `Art_Analysis_Simulation`:

```powershell
cmake -S . -B build_simple
cmake --build build_simple --config Release --target ArtAnalysisSimulation
```

## 4. Exécution

## Mode interactif

```powershell
.\build_simple\Release\ArtAnalysisSimulation.exe
```

## Mode batch (macro)

```powershell
.\build_simple\Release\ArtAnalysisSimulation.exe .\bl4s_ready.mac
```

## Mode visualisation + shell interactif après macro

```powershell
.\build_simple\Release\ArtAnalysisSimulation.exe --vis .\advanced_visualization.mac
```

## 5. Macros utiles

- `bl4s_ready.mac`: acquisition BL4S de base + export CSV.
- `bl4s_multiangle.mac`: acquisitions multi-angles pour reconstruction (tilt mécanique de l'objet via `/art/objectTiltY`).
- `bl4s_scan2d.mac`: scan 2D (translations objet) + multi-angles.
- `animation_particles.mac`: visualisation trajectoires simple/stable.
- `advanced_visualization.mac`: visualisation avancée alignée avec la géométrie actuelle.

Commandes scan utiles:
- `/art/objectShiftX`, `/art/objectShiftY`: translation de l'objet (reconstruction multi-position).
- `/art/objectTiltY`: inclinaison de l'objet (reconstruction multi-angle).
- `/art/beamOffsetX`, `/art/beamOffsetY`: balayage raster du faisceau.

## 6. Sorties d'analyse

La commande `/art/save <file>.csv` exporte:
- observables événement (`theta`, `deltaE`, directions, intersection objet),
- `obj_x_mm`, `obj_y_mm` sont maintenant exportés en repère local objet (cohérent avec `objectShiftX/Y` et `objectTiltY`),
- estimations physiques ajoutées:
  - `x_over_X0_est`
  - `thickness_est_mm`
  - `X0_est_mm`
  - `dedx_est_MeV_per_mm`
  - `mu_est_per_mm`
  - `transmission_est`
  - `inferred_material`
- résumé par cellule de grille (`mean_theta`, `mean_deltaE`, `mean_thickness`, `transmission`, `mu`).
- métriques de transmission enrichies: `transmitted_weight` (niveau événement) et `transmission_energy` (niveau cellule).

## 7. Reconstruction multi-angle

Script:
[reconstruct_multiangle.py](/c:/Users/adamg/3D%20Objects/5.%20Atelier%20Ecole/IQRA/Art_Analysis_Simulation/reconstruct_multiangle.py)

Exemple:

```powershell
python .\reconstruct_multiangle.py --input .\bl4s_multiangle_results.csv --outdir .\reconstruction_output
```

Sorties:
- `reco_areal_map.png`
- `reco_depth_mip.png`
- `reco_diagnostics.png`
- `reco_volume.npy` (+ cartes dérivées)
- `reco_summary.txt`

## 8. Tests

Test rapide compilé dans le CMake principal:

```powershell
cmake --build build_simple --config Release --target bfs_ready_tests
.\build_simple\Release\bfs_ready_tests.exe
```

Pour la suite de tests étendue (unit/integration/perf/physics/regression), voir:
[README_TESTS.md](/c:/Users/adamg/3D%20Objects/5.%20Atelier%20Ecole/IQRA/Art_Analysis_Simulation/README_TESTS.md)

## 9. Dépannage rapide

## Erreur dataset Geant4

Symptôme:
`Missing Geant4 dataset: G4ENSDFSTATEDATA/ENSDFSTATE.dat`

Action:
- charger votre environnement Geant4,
- vérifier que la variable `G4ENSDFSTATEDATA` pointe vers un dossier contenant `ENSDFSTATE.dat`.

## Aucune fenêtre de visualisation

- lancer avec `--vis <macro>` ou en mode interactif,
- vérifier OpenGL/driver,
- essayer `animation_particles.mac` avant `advanced_visualization.mac`.

## 10. Fichiers principaux

- [ArtAnalysisSimulation.cc](/c:/Users/adamg/3D%20Objects/5.%20Atelier%20Ecole/IQRA/Art_Analysis_Simulation/ArtAnalysisSimulation.cc): entrée programme, parsing args, mode batch/vis.
- [ArtDetectorConstruction.cc](/c:/Users/adamg/3D%20Objects/5.%20Atelier%20Ecole/IQRA/Art_Analysis_Simulation/ArtDetectorConstruction.cc): géométrie monde + objet + détecteurs.
- [ArtEventAction.cc](/c:/Users/adamg/3D%20Objects/5.%20Atelier%20Ecole/IQRA/Art_Analysis_Simulation/ArtEventAction.cc): construction observables événement.
- [ArtAnalysisManager.cc](/c:/Users/adamg/3D%20Objects/5.%20Atelier%20Ecole/IQRA/Art_Analysis_Simulation/ArtAnalysisManager.cc): inférence physique + export.
- [ArtUICommands.cc](/c:/Users/adamg/3D%20Objects/5.%20Atelier%20Ecole/IQRA/Art_Analysis_Simulation/ArtUICommands.cc): commandes `/art/*`.
