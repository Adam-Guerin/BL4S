# Script de test pour la simulation am?lior?e
# Test la version corrig?e du main

Write-Host "=== Test de la Simulation Am?lior?e ===" -ForegroundColor Green

# Configuration de l'environnement Geant4 pour cette session
$envScript = Join-Path $PSScriptRoot "..\setup_geant4_env.ps1"
if (Test-Path $envScript) {
    . $envScript
} else {
    Write-Host "? Script d'environnement Geant4 introuvable: $envScript" -ForegroundColor Yellow
}

# Compilation
Write-Host "?? Compilation de la version am?lior?e..." -ForegroundColor Yellow

& "C:\Program Files\CMake\bin\cmake.exe" --build build_simple --config Release

if ($LASTEXITCODE -eq 0) {
    Write-Host "? Compilation r?ussie!" -ForegroundColor Green
    
    # Test avec macro simple
    $simpleMacro = @"
/run/verbose 1
/run/initialize
/gun/particle e-
/gun/energy 100 keV
/gun/position 0 0 -20 cm
/gun/direction 0 0 1
/control/echo "Test de la simulation am?lior?e"
/run/beamOn 10
/art/analyze
/art/save test_results.csv
/control/echo "Test termin?!"
"@
    
    $simpleMacro | Out-File -FilePath "test_improved.mac" -Encoding ascii
    
    Write-Host ""
    Write-Host "?? Test de la simulation..." -ForegroundColor Cyan
    
    try {
        & ".\build_simple\Release\ArtAnalysisSimulation.exe" test_improved.mac
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "? Test r?ussi!" -ForegroundColor Green
            
            if (Test-Path "test_results.csv") {
                Write-Host "?? R?sultats g?n?r?s: test_results.csv" -ForegroundColor Green
                $content = Get-Content "test_results.csv" -Raw
                Write-Host "Contenu du fichier:" -ForegroundColor White
                Write-Host $content -ForegroundColor Gray
            }
        } elseif ($LASTEXITCODE -eq -1073741515) {
            Write-Host "? ?chec: DLL Geant4 introuvable au runtime (code -1073741515)." -ForegroundColor Red
            Write-Host "V?rifiez que geant4-install\\bin est bien dans PATH pour cette session." -ForegroundColor Yellow
        } elseif ($LASTEXITCODE -eq 2) {
            Write-Host "? ?chec: donn?es Geant4 manquantes (ex: ENSDFSTATE.dat)." -ForegroundColor Red
            Write-Host "Installez/extraire les datasets Geant4 puis relancez le test." -ForegroundColor Yellow
        } else {
            Write-Host "? Test termin? avec code: $LASTEXITCODE" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "? Erreur lors du test: $($_.Exception.Message)" -ForegroundColor Red
    }
    
    # Nettoyage
    if (Test-Path "test_improved.mac") {
        Remove-Item "test_improved.mac" -Force
    }
    
} else {
    Write-Host "? Compilation ?chou?e" -ForegroundColor Red
}

Write-Host ""
Write-Host "?? Am?liorations apport?es:" -ForegroundColor Cyan
Write-Host "? Gestion conditionnelle de la visualisation" -ForegroundColor White
Write-Host "? Mode batch d?tect? automatiquement" -ForegroundColor White
Write-Host "? Nettoyage s?curis? des pointeurs" -ForegroundColor White
Write-Host "? Compatible avec tous les modes d'ex?cution" -ForegroundColor White

Write-Host ""
Write-Host "?? Pour l'interface web (toujours disponible):" -ForegroundColor Cyan
Write-Host "start user_interface.html" -ForegroundColor White

Write-Host ""
Write-Host "?? Test termin?!" -ForegroundColor Green

