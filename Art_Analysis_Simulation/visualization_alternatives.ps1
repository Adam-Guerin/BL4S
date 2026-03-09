# Script pour visualisations alternatives quand Geant4 3D ne fonctionne pas
# Offre plusieurs options de visualisation fonctionnelles

Write-Host "=== Visualisations Alternatives Disponibles ===" -ForegroundColor Green

Write-Host ""
Write-Host "🌐 Option 1: Interface Web Interactive (Recommandée)" -ForegroundColor Cyan
Write-Host "   - Interface moderne avec graphiques temps réel" -ForegroundColor White
Write-Host "   - Contrôles interactifs et sliders" -ForegroundColor White
Write-Host "   - Visualisation des couches et pigments" -ForegroundColor White
Write-Host ""

# Lancer l'interface web
try {
    Write-Host "🚀 Lancement de l'interface web..." -ForegroundColor Yellow
    Start-Process "user_interface.html"
    Write-Host "✅ Interface web lancée dans votre navigateur" -ForegroundColor Green
} catch {
    Write-Host "❌ Erreur lors du lancement de l'interface web" -ForegroundColor Red
}

Write-Host ""
Write-Host "📊 Option 2: Rapports Python avec Visualisations" -ForegroundColor Cyan
Write-Host "   - Graphiques matplotlib haute résolution" -ForegroundColor White
Write-Host "   - Analyse des couches et pigments" -ForegroundColor White
Write-Host "   - Rapport détaillé en Markdown" -ForegroundColor White
Write-Host ""

# Générer les rapports
try {
    Write-Host "🚀 Génération des rapports visuels..." -ForegroundColor Yellow
    python generate_report.py
    Write-Host "✅ Rapports générés avec succès" -ForegroundColor Green
    
    # Vérifier les fichiers générés
    if (Test-Path "artwork_layers.png") {
        Write-Host "📈 artwork_layers.png généré" -ForegroundColor Green
    }
    if (Test-Path "scattering_analysis.png") {
        Write-Host "📈 scattering_analysis.png généré" -ForegroundColor Green
    }
    if (Test-Path "artwork_analysis_report.md") {
        Write-Host "📝 artwork_analysis_report.md généré" -ForegroundColor Green
    }
} catch {
    Write-Host "❌ Erreur lors de la génération des rapports" -ForegroundColor Red
}

Write-Host ""
Write-Host "🔬 Option 3: Simulation en Mode Console" -ForegroundColor Cyan
Write-Host "   - Analyse Geant4 complète sans interface graphique" -ForegroundColor White
Write-Host "   - Export des données brutes" -ForegroundColor White
Write-Host "   - Résultats dans fichiers CSV" -ForegroundColor White
Write-Host ""

# Créer une macro de simulation simple
$simulationMacro = @"
# Simulation sans visualisation 3D
/run/verbose 1
/event/verbose 0
/tracking/verbose 0
/run/initialize

# Configuration du faisceau
/gun/particle e-
/gun/energy 100 keV
/gun/position 0 0 -20 cm
/gun/direction 0 0 1

# Lancer la simulation
/control/echo "=== Simulation d'Analyse d'Art ==="
/control/echo "Nombre d'électrons: 1000"
/run/beamOn 1000

# Analyser les résultats
/art/analyze
/art/save simulation_results.csv
/art/report console

/control/echo "Simulation terminée!"
/control/echo "Résultats sauvegardés dans simulation_results.csv"
"@

$simulationMacro | Out-File -FilePath "simulation_console.mac" -Encoding UTF8

# Lancer la simulation console
try {
    Write-Host "🚀 Lancement de la simulation console..." -ForegroundColor Yellow
    & ".\build_simple\Release\ArtAnalysisSimulation.exe" simulation_console.mac
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ Simulation console terminée avec succès" -ForegroundColor Green
        
        if (Test-Path "simulation_results.csv") {
            Write-Host "📊 Résultats disponibles dans simulation_results.csv" -ForegroundColor Green
        }
    } else {
        Write-Host "⚠ Simulation terminée avec code: $LASTEXITCODE" -ForegroundColor Yellow
    }
} catch {
    Write-Host "❌ Erreur lors de la simulation console" -ForegroundColor Red
}

Write-Host ""
Write-Host "📁 Résumé des Fichiers Disponibles:" -ForegroundColor Cyan

# Lister les fichiers générés
$files = @("user_interface.html", "artwork_layers.png", "scattering_analysis.png", 
             "artwork_analysis_report.md", "simulation_results.csv")

foreach ($file in $files) {
    if (Test-Path $file) {
        $size = (Get-Item $file).Length / 1KB
        Write-Host "✅ $file ($([math]::Round($size, 2)) KB)" -ForegroundColor Green
    } else {
        Write-Host "❌ $file (non généré)" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "🎯 Recommandations:" -ForegroundColor Cyan
Write-Host "1. Utilisez l'interface web pour une expérience interactive" -ForegroundColor White
Write-Host "2. Consultez les rapports Python pour des analyses détaillées" -ForegroundColor White
Write-Host "3. Utilisez la simulation console pour des analyses batch" -ForegroundColor White
Write-Host ""

Write-Host "🔧 Pour résoudre le problème Geant4 3D:" -ForegroundColor Yellow
Write-Host "- Mettre à jour les drivers graphiques" -ForegroundColor Gray
Write-Host "- Installer OpenGL 3.3+ support" -ForegroundColor Gray
Write-Host "- Utiliser Xming ou VcXsrv pour X11 forwarding" -ForegroundColor Gray
Write-Host "- Configurer GEANT4_USE_OPENGL_X11" -ForegroundColor Gray

Write-Host ""
Write-Host "🎨 Profitez de toutes les visualisations alternatives!" -ForegroundColor Green
