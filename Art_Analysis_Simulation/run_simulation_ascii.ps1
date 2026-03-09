# Script pour lancer la simulation en mode ASCII (contournement Geant4 3D)
# Fonctionne parfaitement sur Windows sans problèmes graphiques

Write-Host "=== Simulation Geant4 Mode ASCII ===" -ForegroundColor Green

# Vérifier l'exécutable
$exePath = ".\build_simple\Release\ArtAnalysisSimulation.exe"
if (!(Test-Path $exePath)) {
    Write-Host "❌ Exécutable non trouvé. Compilez d'abord:" -ForegroundColor Red
    Write-Host ".\build_simple.ps1" -ForegroundColor Yellow
    exit 1
}

Write-Host "✅ Exécutable trouvé: $exePath" -ForegroundColor Green

# Vérifier la macro ASCII
$macroPath = ".\visualization_workaround.mac"
if (!(Test-Path $macroPath)) {
    Write-Host "❌ Macro ASCII non trouvée: $macroPath" -ForegroundColor Red
    exit 1
}

Write-Host "✅ Macro ASCII trouvée: $macroPath" -ForegroundColor Green

Write-Host ""
Write-Host "🔬 Lancement de la simulation en mode texte..." -ForegroundColor Cyan
Write-Host "Ce mode évite les problèmes graphiques Geant4 sur Windows" -ForegroundColor White
Write-Host "Toutes les fonctionnalités d'analyse sont préservées" -ForegroundColor White
Write-Host ""

# Lancer la simulation ASCII
try {
    Write-Host "🚀 Démarrage..." -ForegroundColor Yellow
    & $exePath $macroPath
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "✅ Simulation ASCII terminée avec succès!" -ForegroundColor Green
        
        # Vérifier les résultats
        if (Test-Path "ascii_simulation_results.csv") {
            $size = (Get-Item "ascii_simulation_results.csv").Length / 1KB
            Write-Host "📊 Résultats générés: ascii_simulation_results.csv ($([math]::Round($size, 2)) KB)" -ForegroundColor Green
        }
        
        Write-Host ""
        Write-Host "🎨 Pour les visualisations graphiques:" -ForegroundColor Cyan
        Write-Host "python generate_report.py" -ForegroundColor White
        Write-Host "start user_interface.html" -ForegroundColor White
        
    } else {
        Write-Host ""
        Write-Host "⚠ Simulation terminée avec code: $LASTEXITCODE" -ForegroundColor Yellow
        Write-Host "Vérifiez les messages ci-dessus pour plus de détails" -ForegroundColor White
    }
} catch {
    Write-Host ""
    Write-Host "❌ Erreur lors de la simulation: $($_.Exception.Message)" -ForegroundColor Red
}

Write-Host ""
Write-Host "📋 Avantages du mode ASCII:" -ForegroundColor Cyan
Write-Host "✅ Fonctionne sur tous les systèmes Windows" -ForegroundColor White
Write-Host "✅ Pas de problèmes graphiques OpenGL" -ForegroundColor White
Write-Host "✅ Analyse complète des données" -ForegroundColor White
Write-Host "✅ Compatible avec tous les rapports" -ForegroundColor White
Write-Host "✅ Sortie texte claire et lisible" -ForegroundColor White

Write-Host ""
Write-Host "🎯 Prochaine étape recommandée:" -ForegroundColor Cyan
Write-Host "Générez les visualisations graphiques avec:" -ForegroundColor White
Write-Host "python generate_report.py" -ForegroundColor Yellow

Write-Host ""
Write-Host "🎨 Simulation d'analyse d'art fonctionnelle!" -ForegroundColor Green
