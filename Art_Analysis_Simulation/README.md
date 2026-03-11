# Art Analysis Simulation

Simulation Geant4 de tomographie par électrons relativistes (style BL4S) avec:
- trackers amont/aval,
- objet multicouche avec inclusions 2D,
- calorimètres amont/aval (ECAL aval dense),
- reconstruction basée sur diffusion multiple + perte d'énergie.

## Macros principales
- `bl4s_ready.mac`: run standard.
- `bl4s_multiangle.mac`: projections angulaires (tilt objet).
- `bl4s_scan2d.mac`: scan 2D (translations objet) + angles.

## Nouvelles commandes utiles
- `/art/objectShiftX <valeur>`
- `/art/objectShiftY <valeur>`
- `/art/objectTiltY <valeur>`
- `/art/beamOffsetX <valeur>`
- `/art/beamOffsetY <valeur>`

## Build
```powershell
cmake -S . -B build_simple
cmake --build build_simple --config Release --target ArtAnalysisSimulation
```

## Exécution (batch)
```powershell
.\build_simple\Release\ArtAnalysisSimulation.exe .\bl4s_scan2d.mac
```
