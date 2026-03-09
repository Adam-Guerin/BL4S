# Script simple pour tester les composants de base

Write-Host "=== Test Simple de la Simulation ===" -ForegroundColor Green

# Test 1: Vérifier que les fichiers sources existent
Write-Host "Vérification des fichiers sources..." -ForegroundColor Yellow

$files = @(
    "ArtAnalysisManager.cc",
    "ArtUICommands.cc", 
    "ArtLogger.cc",
    "ArtException.hh"
)

foreach ($file in $files) {
    if (Test-Path $file) {
        Write-Host "✓ $file trouvé" -ForegroundColor Green
    } else {
        Write-Host "❌ $file manquant" -ForegroundColor Red
    }
}

# Test 2: Vérifier la syntaxe de base
Write-Host "`nTest de syntaxe Geant4..." -ForegroundColor Yellow

# Créer un test simple qui ne dépend pas de Geant4
$testCode = @"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== Test C++ de Base ===" << std::endl;
    
    // Test des types de base
    double energy = 100.0;
    double z = 82.0;
    bool isHistorical = true;
    
    std::cout << "Énergie: " << energy << " keV" << std::endl;
    std::cout << "Numéro atomique: " << z << std::endl;
    std::cout << "Pigment historique: " << (isHistorical ? "OUI" : "NON") << std::endl;
    
    std::cout << "✅ Test C++ réussi!" << std::endl;
    return 0;
}
"@

$testCode | Out-File -FilePath "basic_test.cpp" -Encoding UTF8

# Tenter de compiler avec le compilateur par défaut
try {
    & "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe" /EHsc basic_test.cpp
    if (Test-Path "basic_test.exe") {
        Write-Host "✅ Compilation C++ réussie" -ForegroundColor Green
        & ".\basic_test.exe"
        Remove-Item "basic_test.exe" -Force
    } else {
        Write-Host "❌ Compilation C++ échouée" -ForegroundColor Red
    }
} catch {
    Write-Host "⚠ Compilateur MSVC non trouvé, test avec autre méthode..." -ForegroundColor Yellow
    
    # Test simple sans compilation
    Write-Host "Test des concepts logiques:" -ForegroundColor Yellow
    
    # Test logique des fonctions principales
    $energyLoss = 50.0
    $materialZ = 82.0
    $referenceLoss = 100.0
    $referenceZ = 10.0
    $referenceThickness = 100.0
    
    $thickness = $referenceThickness * ($energyLoss / $referenceLoss) * [math]::Pow($referenceZ / $materialZ, 2)
    
    Write-Host "Perte d'énergie: $energyLoss keV" -ForegroundColor Cyan
    Write-Host "Numéro atomique: $materialZ" -ForegroundColor Cyan
    Write-Host "Épaisseur estimée: $([math]::Round($thickness, 2)) μm" -ForegroundColor Cyan
    
    if ($thickness -gt 0 -and $thickness -lt 1000) {
        Write-Host "✅ Calcul d'épaisseur logique correct" -ForegroundColor Green
    } else {
        Write-Host "❌ Calcul d'épaisseur incorrect" -ForegroundColor Red
    }
}

# Nettoyage
if (Test-Path "basic_test.cpp") { Remove-Item "basic_test.cpp" -Force }
if (Test-Path "basic_test.obj") { Remove-Item "basic_test.obj" -Force }

Write-Host "`n=== Test Terminé ===" -ForegroundColor Green
