# Script d'installation et configuration de l'environnement pour la simulation
# Vérifie les dépendances et configure l'environnement Geant4

Write-Host "=== Configuration de l'Environnement de Simulation d'Art ===" -ForegroundColor Green

# Vérification des dépendances
Write-Host "Vérification des dépendances..." -ForegroundColor Yellow

# Vérifier CMake
try {
    $cmakeVersion = & "C:\Program Files\CMake\bin\cmake.exe" --version | Select-Object -First 1
    Write-Host "✓ CMake trouvé: $cmakeVersion" -ForegroundColor Green
} catch {
    Write-Host "✗ CMake non trouvé. Veuillez installer CMake depuis https://cmake.org/" -ForegroundColor Red
    exit 1
}

# Vérifier Visual Studio Build Tools
try {
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vsWhere) {
        $vsPath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($vsPath) {
            Write-Host "✓ Visual Studio Build Tools trouvé" -ForegroundColor Green
        } else {
            throw "VS Build Tools not found"
        }
    } else {
        throw "vswhere.exe not found"
    }
} catch {
    Write-Host "✗ Visual Studio Build Tools non trouvé. Veuillez installer Build Tools for Visual Studio" -ForegroundColor Red
    exit 1
}

# Vérifier Geant4
$G4INSTALL = "C:\Users\adamg\3D Objects\5. Atelier Ecole\IQRA\geant4-install"
if (Test-Path $G4INSTALL) {
    Write-Host "✓ Geant4 trouvé dans: $G4INSTALL" -ForegroundColor Green
} else {
    Write-Host "✗ Geant4 non trouvé dans: $G4INSTALL" -ForegroundColor Red
    Write-Host "Veuillez d'abord installer Geant4 ou modifier le chemin dans ce script" -ForegroundColor Yellow
    exit 1
}

# Configuration des variables d'environnement
Write-Host "Configuration des variables d'environnement..." -ForegroundColor Yellow

$env:GEANT4_INSTALL = $G4INSTALL
$env:PATH = "$G4INSTALL\bin;$env:PATH"
$env:GEANT4_DIR = "$G4INSTALL\lib\cmake\Geant4"

# Variables persistantes pour la session
[System.Environment]::SetEnvironmentVariable("GEANT4_INSTALL", $G4INSTALL, "User")
[System.Environment]::SetEnvironmentVariable("GEANT4_DIR", "$G4INSTALL\lib\cmake\Geant4", "User")

Write-Host "Variables d'environnement configurées:" -ForegroundColor Cyan
Write-Host "GEANT4_INSTALL = $env:GEANT4_INSTALL" -ForegroundColor White
Write-Host "GEANT4_DIR = $env:GEANT4_DIR" -ForegroundColor White

# Vérification des dépendances Python pour les rapports
Write-Host "Vérification des dépendances Python..." -ForegroundColor Yellow

try {
    $pythonVersion = python --version 2>&1
    Write-Host "✓ Python trouvé: $pythonVersion" -ForegroundColor Green
    
    # Vérifier les packages Python requis
    $requiredPackages = @("matplotlib", "numpy", "pandas", "seaborn")
    foreach ($package in $requiredPackages) {
        try {
            python -c "import $package" 2>$null
            Write-Host "✓ Package Python '$package' disponible" -ForegroundColor Green
        } catch {
            Write-Host "✗ Package Python '$package' manquant. Installation..." -ForegroundColor Yellow
            python -m pip install $package
        }
    }
} catch {
    Write-Host "✗ Python non trouvé. Les rapports ne pourront pas être générés." -ForegroundColor Yellow
    Write-Host "Installez Python depuis https://python.org/" -ForegroundColor Cyan
}

# Création des répertoires de travail
Write-Host "Création des répertoires de travail..." -ForegroundColor Yellow

$directories = @("build", "results", "data", "reports")
foreach ($dir in $directories) {
    if (!(Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir | Out-Null
        Write-Host "✓ Répertoire créé: $dir" -ForegroundColor Green
    } else {
        Write-Host "✓ Répertoire existant: $dir" -ForegroundColor Cyan
    }
}

# Test de compilation rapide
Write-Host "Test de compilation rapide..." -ForegroundColor Yellow

try {
    Push-Location build
    & "C:\Program Files\CMake\bin\cmake.exe" -DGeant4_DIR=$env:GEANT4_DIR -S .. -B . --config Release
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ Configuration CMake réussie" -ForegroundColor Green
    } else {
        throw "CMake configuration failed"
    }
    Pop-Location
} catch {
    Write-Host "✗ Erreur de configuration CMake" -ForegroundColor Red
    exit 1
}

# Résumé
Write-Host ""
Write-Host "=== Configuration Terminée avec Succès! ===" -ForegroundColor Green
Write-Host ""
Write-Host "Prochaines étapes:" -ForegroundColor Cyan
Write-Host "1. Exécutez .\build_and_run.ps1 pour compiler et lancer la simulation" -ForegroundColor White
Write-Host "2. Ou utilisez .\quick_test.ps1 pour un test rapide" -ForegroundColor White
Write-Host "3. Les résultats seront sauvegardés dans le répertoire 'results/'" -ForegroundColor White
Write-Host ""
Write-Host "Pour modifier la simulation:" -ForegroundColor Cyan
Write-Host "- Éditez les fichiers .mac pour changer les paramètres" -ForegroundColor White
Write-Host "- Modifiez ArtDetectorConstruction.cc pour changer les matériaux" -ForegroundColor White
Write-Host "- Utilisez generate_report.py pour créer des rapports visuels" -ForegroundColor White
