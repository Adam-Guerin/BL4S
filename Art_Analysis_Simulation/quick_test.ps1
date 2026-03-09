# Script de test rapide pour valider la simulation
# Exécute une simulation courte avec visualisation de base

Write-Host "=== Test Rapide de Simulation d'Art ===" -ForegroundColor Green

# Configuration rapide de l'environnement
$G4INSTALL = "C:\Users\adamg\3D Objects\5. Atelier Ecole\IQRA\geant4-install"
$env:GEANT4_INSTALL = $G4INSTALL
$env:PATH = "$G4INSTALL\bin;$env:PATH"
$env:GEANT4_DIR = "$G4INSTALL\lib\cmake\Geant4"

# Vérification si build existe
if (!(Test-Path "build")) {
    Write-Host "Compilation nécessaire..." -ForegroundColor Yellow
    & .\setup_environment.ps1
}

# Vérification si l'exécutable existe
$exePath = "build\Release\ArtAnalysisSimulation.exe"
if (!(Test-Path $exePath)) {
    Write-Host "Compilation de la simulation..." -ForegroundColor Yellow
    Push-Location build
    & "C:\Program Files\CMake\bin\cmake.exe" --build . --config Release
    Pop-Location
}

if (Test-Path $exePath) {
    Write-Host "Lancement du test rapide..." -ForegroundColor Yellow
    Write-Host "Simulation de 1000 électrons avec visualisation" -ForegroundColor Cyan
    
    # Créer une macro de test rapide
    $testMacro = @"
# Macro de test rapide
/run/verbose 1
/event/verbose 0
/tracking/verbose 0
/run/initialize

# Visualisation de base
/vis/open OGL 800x600-0+0
/vis/drawVolume
/vis/viewer/set/viewpointVector -1 0 0
/vis/viewer/set/style surface
/vis/viewer/set/auxiliaryEdge true

# Configuration du faisceau
/gun/particle e-
/gun/energy 100 keV
/gun/direction 0 0 1
/gun/position 0 0 -20 cm

# Simulation courte
/run/beamOn 1000

# Attendre un peu avant de fermer
/sleep 5
"@
    
    $testMacro | Out-File -FilePath "quick_test.mac" -Encoding UTF8
    
    # Lancer la simulation
    & $exePath quick_test.mac
    
    Write-Host "Test terminé!" -ForegroundColor Green
    
    # Nettoyage
    if (Test-Path "quick_test.mac") {
        Remove-Item "quick_test.mac"
    }
    
} else {
    Write-Host "ERREUR: Impossible de trouver l'exécutable de simulation" -ForegroundColor Red
    Write-Host "Veuillez exécuter .\setup_environment.ps1 d'abord" -ForegroundColor Yellow
}
