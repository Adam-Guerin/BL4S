# Script PowerShell pour compiler et exécuter la simulation d'analyse d'œuvre d'art

Write-Host "=== Simulation d'Analyse d'Œuvre d'Art par Faisceau d'Électrons ===" -ForegroundColor Green

# Configuration de l'environnement Geant4
$G4INSTALL = "C:\Users\adamg\3D Objects\5. Atelier Ecole\IQRA\geant4-install"
$env:GEANT4_INSTALL = $G4INSTALL
$env:PATH = "$G4INSTALL\bin;$env:PATH"
$env:GEANT4_DIR = "$G4INSTALL\lib\cmake\Geant4"

Write-Host "Environnement Geant4 configuré:" -ForegroundColor Yellow
Write-Host "GEANT4_INSTALL = $env:GEANT4_INSTALL" -ForegroundColor Cyan

# Création du répertoire de build
$BUILD_DIR = "build"
if (Test-Path $BUILD_DIR) {
    Remove-Item -Recurse -Force $BUILD_DIR
}
New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null

Write-Host "Création du répertoire de build: $BUILD_DIR" -ForegroundColor Yellow

# Configuration avec CMake
Write-Host "Configuration avec CMake..." -ForegroundColor Yellow
& "C:\Program Files\CMake\bin\cmake.exe" -DGeant4_DIR=$env:GEANT4_DIR -S . -B $BUILD_DIR

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERREUR: Configuration CMake échouée" -ForegroundColor Red
    exit 1
}

# Compilation
Write-Host "Compilation de la simulation..." -ForegroundColor Yellow
& "C:\Program Files\CMake\bin\cmake.exe" --build $BUILD_DIR --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERREUR: Compilation échouée" -ForegroundColor Red
    exit 1
}

Write-Host "Compilation réussie!" -ForegroundColor Green

# Exécution de la simulation
Write-Host "Lancement de la simulation..." -ForegroundColor Yellow
Write-Host "Analyse de 10000 électrons sur l'œuvre d'art..." -ForegroundColor Cyan

& "$BUILD_DIR\Release\ArtAnalysisSimulation.exe" run_analysis.mac

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERREUR: Exécution de la simulation échouée" -ForegroundColor Red
    exit 1
}

Write-Host "Simulation terminée!" -ForegroundColor Green

# Vérification des résultats
if (Test-Path "results_art_analysis.csv") {
    Write-Host "Résultats sauvegardés dans: results_art_analysis.csv" -ForegroundColor Green
    Write-Host "Vous pouvez maintenant analyser les données avec Excel ou un autre outil." -ForegroundColor Cyan
} else {
    Write-Host "ATTENTION: Fichier de résultats non trouvé" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Pour la visualisation interactive:" -ForegroundColor Cyan
Write-Host ".\build\Release\ArtAnalysisSimulation.exe" -ForegroundColor White
Write-Host ""
Write-Host "Pour modifier les paramètres:" -ForegroundColor Cyan
Write-Host "- Éditez run_analysis.mac pour changer le nombre d'électrons" -ForegroundColor White
Write-Host "- Modifiez ArtDetectorConstruction.cc pour changer les matériaux" -ForegroundColor White
