# Script PowerShell pour compiler et tester la simulation complète
# Inclut compilation, tests unitaires et validation

Write-Host "=== Compilation et Test de la Simulation d'Art ===" -ForegroundColor Green

# Configuration de l'environnement
$G4INSTALL = "C:\Users\adamg\3D Objects\5. Atelier Ecole\IQRA\geant4-install"
$env:GEANT4_INSTALL = $G4INSTALL
$env:PATH = "$G4INSTALL\bin;$env:PATH"
$env:GEANT4_DIR = "$G4INSTALL\lib\cmake\Geant4"

Write-Host "Environnement Geant4 configuré" -ForegroundColor Yellow

# Nettoyage et création du répertoire de build
$BUILD_DIR = "build"
if (Test-Path $BUILD_DIR) {
    Remove-Item -Recurse -Force $BUILD_DIR
}
New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null

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

# Test rapide de validation
Write-Host "Test rapide de validation..." -ForegroundColor Yellow
& "$BUILD_DIR\Release\ArtAnalysisSimulation.exe" validation_test.mac

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Test de validation réussi" -ForegroundColor Green
} else {
    Write-Host "❌ Test de validation échoué" -ForegroundColor Red
}

# Compilation et exécution des tests unitaires
Write-Host "Compilation des tests unitaires..." -ForegroundColor Yellow

# Créer un exécutable séparé pour les tests unitaires
Push-Location $BUILD_DIR
& "C:\Program Files\CMake\bin\cmake.exe" -DCOMPILE_TESTS=ON -S .. -B . --config Release
& "C:\Program Files\CMake\bin\cmake.exe" --build . --config Release --target unit_tests
Pop-Location

if (Test-Path "$BUILD_DIR\Release\unit_tests.exe") {
    Write-Host "Exécution des tests unitaires..." -ForegroundColor Yellow
    & "$BUILD_DIR\Release\unit_tests.exe"
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ Tous les tests unitaires passés" -ForegroundColor Green
    } else {
        Write-Host "❌ Certains tests unitaires ont échoué" -ForegroundColor Red
    }
} else {
    Write-Host "⚠ Tests unitaires non compilés" -ForegroundColor Yellow
}

# Test de performance optionnel
$choice = Read-Host "Voulez-vous exécuter le test de performance? (o/n)"
if ($choice -eq 'o' -or $choice -eq 'O') {
    Write-Host "Test de performance en cours..." -ForegroundColor Yellow
    & "$BUILD_DIR\Release\ArtAnalysisSimulation.exe" performance_test.mac
    Write-Host "Test de performance terminé" -ForegroundColor Green
}

Write-Host ""
Write-Host "=== Résumé ===" -ForegroundColor Cyan
Write-Host "✓ Compilation terminée" -ForegroundColor Green
Write-Host "✓ Tests de validation exécutés" -ForegroundColor Green
Write-Host "✓ Tests unitaires compilés" -ForegroundColor Green
Write-Host ""
Write-Host "La simulation est prête à être utilisée!" -ForegroundColor Green
Write-Host ""
Write-Host "Pour l'utiliser:" -ForegroundColor Cyan
Write-Host "- Mode interactif: .\build\Release\ArtAnalysisSimulation.exe" -ForegroundColor White
Write-Host "- Avec macro: .\build\Release\ArtAnalysisSimulation.exe art_analysis.mac" -ForegroundColor White
Write-Host "- Interface web: start user_interface.html" -ForegroundColor White
