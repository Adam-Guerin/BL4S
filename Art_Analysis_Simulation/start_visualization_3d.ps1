# Script pour lancer la visualisation 3D de la simulation
# Ouvre la fenetre 3D interactive avec Geant4

Write-Host "=== Demarrage de la Visualisation 3D ===" -ForegroundColor Green

# Charger l'environnement Geant4
$envScript = Join-Path $PSScriptRoot "..\setup_geant4_env.ps1"
if (Test-Path $envScript) {
    . $envScript
}

# Reparer dataset bloquant (ENSDFSTATE)
$repairScript = Join-Path $PSScriptRoot "repair_geant4_data.ps1"
if (Test-Path $repairScript) {
    & $repairScript
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERREUR: Reparation des donnees impossible, arret." -ForegroundColor Red
        exit 1
    }
}

# V?rifier si l'ex?cutable existe
$exePath = ".\build_simple\Release\ArtAnalysisSimulation.exe"
if (!(Test-Path $exePath)) {
    Write-Host "ERREUR: Executable non trouve. Veuillez d'abord compiler." -ForegroundColor Red
    Write-Host "Compilation automatique en cours..." -ForegroundColor Yellow
    & "C:\Program Files\CMake\bin\cmake.exe" --build build_simple --config Release
    if ($LASTEXITCODE -ne 0 -or !(Test-Path $exePath)) {
        Write-Host "ERREUR: Compilation echouee." -ForegroundColor Red
        exit 1
    }
}

Write-Host "OK: Executable trouve: $exePath" -ForegroundColor Green

# Verifier si la macro de visualisation existe
$macroPath = ".\animation_particles.mac"
if (!(Test-Path $macroPath)) {
    Write-Host "ERREUR: Macro de visualisation non trouvee: $macroPath" -ForegroundColor Red
    exit 1
}

Write-Host "OK: Macro de visualisation trouvee: $macroPath" -ForegroundColor Green

Write-Host ""
Write-Host "Demarrage de la visualisation 3D..." -ForegroundColor Cyan
Write-Host "Fenetre: 1280x820 pixels" -ForegroundColor White
Write-Host "Commandes utiles:" -ForegroundColor White
Write-Host "  - Clic gauche + glissement: Rotation" -ForegroundColor Gray
Write-Host "  - Clic droit + glissement: Zoom" -ForegroundColor Gray
Write-Host "  - Molette: Zoom in/out" -ForegroundColor Gray
Write-Host "  - Ctrl + clic droit: Pan" -ForegroundColor Gray
Write-Host "  - ESC: Quitter" -ForegroundColor Gray
Write-Host ""
Write-Host "Commandes Geant4 disponibles:" -ForegroundColor White
Write-Host "  /vis/viewer/zoom - Zoomer" -ForegroundColor Gray
Write-Host "  /vis/viewer/pan - Deplacer la vue" -ForegroundColor Gray
Write-Host "  /vis/viewer/rotate - Rotation automatique" -ForegroundColor Gray
Write-Host "  /vis/viewer/set/cutaway false - Desactiver coupe" -ForegroundColor Gray
Write-Host "  /vis/trajectory/clear - Effacer trajectoires" -ForegroundColor Gray
Write-Host "  /run/beamOn 1000 - Ajouter 1000 electrons" -ForegroundColor Gray
Write-Host ""

# Lancer la visualisation
try {
    Write-Host "Lancement de la simulation 3D..." -ForegroundColor Yellow
    & $exePath --vis $macroPath
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "OK: Visualisation 3D terminee avec succes." -ForegroundColor Green
    } else {
        Write-Host ""
        Write-Host "ATTENTION: Visualisation terminee avec code: $LASTEXITCODE" -ForegroundColor Yellow
    }
} catch {
    Write-Host ""
    Write-Host "ERREUR lors du lancement: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host ""
    Write-Host "Solutions possibles:" -ForegroundColor Yellow
    Write-Host "1. Verifier que Geant4 est bien installe" -ForegroundColor White
    Write-Host "2. Verifier les drivers graphiques (OpenGL)" -ForegroundColor White
    Write-Host "3. Essayer en mode fenetre plus petite" -ForegroundColor White
}

Write-Host ""
Write-Host "Pour generer des rapports visuels:" -ForegroundColor Cyan
Write-Host "python generate_report.py" -ForegroundColor White

Write-Host ""
Write-Host "Pour l'interface web:" -ForegroundColor Cyan
Write-Host "start user_interface.html" -ForegroundColor White

Write-Host ""
Write-Host "Visualisation prete." -ForegroundColor Green

