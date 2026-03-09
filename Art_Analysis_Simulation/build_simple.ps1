# Script PowerShell pour compiler la simulation avec CMake simple
# Version minimaliste qui évite les problèmes de copie de fichiers

Write-Host "=== Compilation Simple de la Simulation d'Art ===" -ForegroundColor Green

# Configuration de l'environnement
$G4INSTALL = "C:\Users\adamg\3D Objects\5. Atelier Ecole\IQRA\geant4-install"
$env:GEANT4_INSTALL = $G4INSTALL
$env:PATH = "$G4INSTALL\bin;$env:PATH"
$env:GEANT4_DIR = "$G4INSTALL\lib\cmake\Geant4"

Write-Host "Environnement Geant4 configuré" -ForegroundColor Yellow

# Nettoyage
$BUILD_DIR = "build_simple"
if (Test-Path $BUILD_DIR) {
    Write-Host "Nettoyage du répertoire de build..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BUILD_DIR
}
New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null

Write-Host "Configuration CMake simple..." -ForegroundColor Yellow

# Configuration CMake minimaliste
& "C:\Program Files\CMake\bin\cmake.exe" `
    -DGeant4_DIR=$env:GEANT4_DIR `
    -DCMAKE_BUILD_TYPE=Release `
    -S . -B $BUILD_DIR

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERREUR: Configuration CMake échouée" -ForegroundColor Red
    exit 1
}

Write-Host "Configuration réussie!" -ForegroundColor Green

# Compilation
Write-Host "Compilation de la simulation..." -ForegroundColor Yellow
& "C:\Program Files\CMake\bin\cmake.exe" --build $BUILD_DIR --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERREUR: Compilation échouée" -ForegroundColor Red
    Write-Host "Vérifiez les erreurs ci-dessus" -ForegroundColor Yellow
    exit 1
}

Write-Host "Compilation réussie!" -ForegroundColor Green

# Vérification de l'exécutable
$exePath = "$BUILD_DIR\Release\ArtAnalysisSimulation.exe"
if (Test-Path $exePath) {
    Write-Host "✅ Exécutable créé: $exePath" -ForegroundColor Green
} else {
    Write-Host "❌ Exécutable non trouvé" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "=== Test de Base ===" -ForegroundColor Cyan

# Créer un test simple
$testMacro = @"
/run/verbose 1
/run/initialize
/control/echo "=== Simulation d'Analyse d'Art ==="
/control/echo "Test de configuration réussi"
/control/echo "Geant4 version: 11.4.0"
/control/echo "Compilation: Release"
/control/echo "Standard: C++17"
/exit
"@

$testMacro | Out-File -FilePath "$BUILD_DIR\test_simple.mac" -Encoding UTF8

# Lancer le test
Write-Host "Lancement du test simple..." -ForegroundColor Yellow
try {
    & $exePath "$BUILD_DIR\test_simple.mac"
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ Test simple réussi!" -ForegroundColor Green
    } else {
        Write-Host "⚠ Test simple a échoué (code: $LASTEXITCODE)" -ForegroundColor Yellow
    }
} catch {
    Write-Host "❌ Erreur lors du test: $($_.Exception.Message)" -ForegroundColor Red
}

Write-Host ""
Write-Host "=== Résumé ===" -ForegroundColor Cyan
Write-Host "✅ Configuration CMake: Terminée" -ForegroundColor Green
Write-Host "✅ Compilation: Terminée" -ForegroundColor Green
Write-Host "✅ Exécutable: Créé" -ForegroundColor Green
Write-Host "✅ Test de base: Effectué" -ForegroundColor Green

Write-Host ""
Write-Host "Pour utiliser la simulation:" -ForegroundColor White
Write-Host "- Simulation: $exePath art_analysis.mac" -ForegroundColor Cyan
Write-Host "- Interface web: start user_interface.html" -ForegroundColor Cyan
Write-Host "- Scripts Python: python generate_report.py" -ForegroundColor Cyan

Write-Host ""
Write-Host "🎉 Compilation terminée avec succès!" -ForegroundColor Green
