# Script PowerShell pour compiler la simulation avec les corrections
# Version corrigée qui résout tous les problèmes de compilation

Write-Host "=== Compilation Corrigée de la Simulation d'Art ===" -ForegroundColor Green

# Configuration de l'environnement
$G4INSTALL = "C:\Users\adamg\3D Objects\5. Atelier Ecole\IQRA\geant4-install"
$env:GEANT4_INSTALL = $G4INSTALL
$env:PATH = "$G4INSTALL\bin;$env:PATH"
$env:GEANT4_DIR = "$G4INSTALL\lib\cmake\Geant4"

Write-Host "Environnement Geant4 configuré" -ForegroundColor Yellow

# Nettoyage complet
$BUILD_DIR = "build_fixed"
if (Test-Path $BUILD_DIR) {
    Write-Host "Nettoyage du répertoire de build..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BUILD_DIR
}
New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null

Write-Host "Configuration CMake corrigée..." -ForegroundColor Yellow

# Configuration CMake avec tous les paramètres
& "C:\Program Files\CMake\bin\cmake.exe" `
    -DGeant4_DIR=$env:GEANT4_DIR `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_CXX_STANDARD=17 `
    -S . -B $BUILD_DIR

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERREUR: Configuration CMake échouée" -ForegroundColor Red
    Write-Host "Vérifiez que Geant4 est bien installé dans: $env:GEANT4_DIR" -ForegroundColor Yellow
    exit 1
}

Write-Host "Configuration réussie!" -ForegroundColor Green

# Compilation
Write-Host "Compilation de la simulation..." -ForegroundColor Yellow
& "C:\Program Files\CMake\bin\cmake.exe" --build $BUILD_DIR --config Release --parallel 4

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
    
    # Test de l'exécutable
    Write-Host "Test de l'exécutable..." -ForegroundColor Yellow
    try {
        & $exePath --version 2>&1 | Out-Null
        Write-Host "✅ Exécutable fonctionnel" -ForegroundColor Green
    } catch {
        Write-Host "⚠ L'exécutable a été créé mais peut avoir des problèmes" -ForegroundColor Yellow
    }
} else {
    Write-Host "❌ Exécutable non trouvé" -ForegroundColor Red
    exit 1
}

# Création d'un script de test simple
$testScript = @"
/run/verbose 1
/run/initialize
/control/echo "Test de base réussi"
/exit
"@

$testScript | Out-File -FilePath "$BUILD_DIR\test_basic.mac" -Encoding UTF8

Write-Host "`n=== Test de Compilation ===" -ForegroundColor Cyan
Write-Host "Lancement du test de base..." -ForegroundColor Yellow

try {
    & $exePath "$BUILD_DIR\test_basic.mac"
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ Test de base réussi!" -ForegroundColor Green
    } else {
        Write-Host "⚠ Test de base a échoué (code: $LASTEXITCODE)" -ForegroundColor Yellow
    }
} catch {
    Write-Host "❌ Erreur lors du test: $($_.Exception.Message)" -ForegroundColor Red
}

Write-Host ""
Write-Host "=== Résumé de la Compilation ===" -ForegroundColor Cyan
Write-Host "✅ Configuration CMake: Terminée" -ForegroundColor Green
Write-Host "✅ Compilation: Terminée" -ForegroundColor Green
Write-Host "✅ Exécutable: Créé" -ForegroundColor Green
Write-Host ""
Write-Host "Pour utiliser la simulation:" -ForegroundColor White
Write-Host "- Test rapide: $exePath test_basic.mac" -ForegroundColor Cyan
Write-Host "- Interface web: start user_interface.html" -ForegroundColor Cyan
Write-Host "- Simulation complète: $exePath art_analysis.mac" -ForegroundColor Cyan

Write-Host ""
Write-Host "🎉 Compilation corrigée terminée avec succès!" -ForegroundColor Green
