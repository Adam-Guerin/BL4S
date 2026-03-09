#include <iostream>
#include <cassert>
#include "ArtAnalysisManager.hh"
#include "ArtLogger.hh"
#include "G4SystemOfUnits.hh"

void test_material_z_calculation() {
    std::cout << "Testing material Z calculation..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test known materials
    assert(manager->CalculateMaterialZ("LeadWhite") == 82);
    assert(manager->CalculateMaterialZ("Vermilion") == 80);
    assert(manager->CalculateMaterialZ("LapisLazuli") == 14);
    assert(manager->CalculateMaterialZ("Malachite") == 29);
    assert(manager->CalculateMaterialZ("TinYellow") == 50);
    
    std::cout << "✓ Material Z calculation tests passed" << std::endl;
}

void test_historical_pigment_detection() {
    std::cout << "Testing historical pigment detection..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test historical pigments
    assert(manager->IsHistoricalPigment("LeadWhite") == true);
    assert(manager->IsHistoricalPigment("Vermilion") == true);
    assert(manager->IsHistoricalPigment("LapisLazuli") == true);
    assert(manager->IsHistoricalPigment("Malachite") == true);
    assert(manager->IsHistoricalPigment("TinYellow") == true);
    
    // Test modern materials
    assert(manager->IsHistoricalPigment("ModernPaint") == false);
    assert(manager->IsHistoricalPigment("Canvas") == false);
    assert(manager->IsHistoricalPigment("Varnish") == false);
    
    std::cout << "✓ Historical pigment detection tests passed" << std::endl;
}

void test_density_calculation() {
    std::cout << "Testing density calculation..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test known densities
    assert(std::abs(manager->CalculateDensity("LeadWhite")/(g/cm3) - 6.5) < 0.01);
    assert(std::abs(manager->CalculateDensity("Vermilion")/(g/cm3) - 8.1) < 0.01);
    assert(std::abs(manager->CalculateDensity("LapisLazuli")/(g/cm3) - 2.4) < 0.01);
    assert(std::abs(manager->CalculateDensity("Malachite")/(g/cm3) - 4.0) < 0.01);
    assert(std::abs(manager->CalculateDensity("TinYellow")/(g/cm3) - 6.8) < 0.01);
    
    std::cout << "✓ Density calculation tests passed" << std::endl;
}

void test_thickness_estimation() {
    std::cout << "Testing thickness estimation..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test thickness estimation
    G4double energyLoss = 50*keV;
    G4double Z = 82; // Lead
    
    G4double thickness = manager->EstimateThickness(energyLoss, Z);
    
    // Should be reasonable for lead with 50 keV loss
    assert(thickness > 0);
    assert(thickness < 1*mm);
    
    std::cout << "✓ Thickness estimation tests passed" << std::endl;
}

void test_stopping_power() {
    std::cout << "Testing stopping power calculation..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test stopping power for known materials
    G4double sp_lead = manager->CalculateStoppingPower("LeadWhite", 100*keV);
    G4double sp_water = manager->CalculateStoppingPower("Water", 100*keV);
    
    // Lead should have higher stopping power than water
    assert(sp_lead > sp_water);
    assert(sp_lead > 0);
    assert(sp_water > 0);
    
    std::cout << "✓ Stopping power tests passed" << std::endl;
}

void run_all_tests() {
    std::cout << "\n=== Running Unit Tests ===" << std::endl;
    
    try {
        test_material_z_calculation();
        test_historical_pigment_detection();
        test_density_calculation();
        test_thickness_estimation();
        test_stopping_power();
        
        std::cout << "\n🎉 All tests passed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ Test failed: " << e.what() << std::endl;
        exit(1);
    }
}

int main() {
    // Initialize logger
    ArtLogger::Instance()->SetLogLevel(LogLevel::INFO);
    
    run_all_tests();
    
    return 0;
}
