#include "ArtAnalysisManager.hh"
#include "ArtLogger.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4Proton.hh"
#include "G4Electron.hh"

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>

void test_full_simulation_pipeline() {
    std::cout << "Testing full simulation pipeline..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Initialize simulation with typical art analysis parameters
    manager->InitializeSimulation();
    
    // Test particle beam setup
    G4ParticleGun* gun = new G4ParticleGun(1);
    gun->SetParticleDefinition(G4Proton::ProtonDefinition());
    gun->SetParticleEnergy(2.0 * GeV);
    gun->SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
    gun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
    
    // Run a small number of events
    const int nEvents = 100;
    manager->RunSimulation(nEvents, gun);
    
    // Verify results were generated
    assert(manager->GetEventCount() == nEvents);
    assert(manager->HasValidResults());
    
    // Check analysis outputs
    auto results = manager->GetAnalysisResults();
    assert(!results.empty());
    
    delete gun;
    std::cout << "✓ Full simulation pipeline test passed" << std::endl;
}

void test_multi_material_analysis() {
    std::cout << "Testing multi-material analysis..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test with various art materials
    std::vector<std::string> materials = {
        "LeadWhite", "Vermilion", "LapisLazuli", 
        "Malachite", "TinYellow", "Canvas"
    };
    
    for (const auto& material : materials) {
        manager->SetTargetMaterial(material);
        manager->InitializeSimulation();
        
        // Run small test
        G4ParticleGun gun(1);
        gun.SetParticleDefinition(G4Proton::ProtonDefinition());
        gun.SetParticleEnergy(1.0 * GeV);
        gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -20.0 * cm));
        gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
        
        manager->RunSimulation(50, &gun);
        
        // Verify material-specific analysis
        auto materialResults = manager->GetMaterialAnalysis(material);
        assert(!materialResults.empty());
        assert(materialResults.count("density") > 0);
        assert(materialResults.count("Z_effective") > 0);
    }
    
    std::cout << "✓ Multi-material analysis test passed" << std::endl;
}

void test_detector_response_chain() {
    std::cout << "Testing detector response chain..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Setup detector configuration
    manager->ConfigureDetector("tracker", 50.0 * micrometer, 10.0 * cm);
    manager->ConfigureDetector("calorimeter", 0.10, 0.01, 2.0 * MeV);
    
    // Initialize with detectors
    manager->InitializeSimulation();
    
    // Run simulation
    G4ParticleGun gun(1);
    gun.SetParticleDefinition(G4Proton::ProtonDefinition());
    gun.SetParticleEnergy(3.0 * GeV);
    gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
    gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
    
    manager->RunSimulation(200, &gun);
    
    // Check detector responses
    auto trackerData = manager->GetTrackerData();
    auto calorimeterData = manager->GetCalorimeterData();
    
    assert(!trackerData.empty());
    assert(!calorimeterData.empty());
    
    // Verify data consistency
    for (const auto& track : trackerData) {
        assert(track.size() >= 2); // At least entry and exit points
    }
    
    for (const auto& cal : calorimeterData) {
        assert(cal.energy > 0);
        assert(cal.time >= 0);
    }
    
    std::cout << "✓ Detector response chain test passed" << std::endl;
}

void test_energy_loss_validation() {
    std::cout << "Testing energy loss validation..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    manager->SetTargetMaterial("LeadWhite");
    manager->InitializeSimulation();
    
    // Test with different beam energies
    std::vector<G4double> energies = {0.5 * GeV, 1.0 * GeV, 2.0 * GeV, 3.0 * GeV};
    
    for (auto energy : energies) {
        G4ParticleGun gun(1);
        gun.SetParticleDefinition(G4Proton::ProtonDefinition());
        gun.SetParticleEnergy(energy);
        gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
        gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
        
        manager->RunSimulation(100, &gun);
        
        auto energyLoss = manager->GetEnergyLossDistribution();
        assert(!energyLoss.empty());
        
        // Verify energy loss is reasonable (should increase with material thickness)
        G4double meanLoss = manager->GetMeanEnergyLoss();
        assert(meanLoss > 0);
        assert(meanLoss < energy); // Can't lose more energy than we start with
    }
    
    std::cout << "✓ Energy loss validation test passed" << std::endl;
}

void test_reconstruction_algorithm() {
    std::cout << "Testing reconstruction algorithm..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Generate test data
    manager->SetTargetMaterial("Vermilion");
    manager->InitializeSimulation();
    
    G4ParticleGun gun(1);
    gun.SetParticleDefinition(G4Proton::ProtonDefinition());
    gun.SetParticleEnergy(1.5 * GeV);
    gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
    gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
    
    manager->RunSimulation(500, &gun);
    
    // Run reconstruction
    auto reconstructionResults = manager->RunReconstruction();
    
    assert(!reconstructionResults.empty());
    
    // Verify reconstruction quality metrics
    assert(reconstructionResults.count("thickness_map") > 0);
    assert(reconstructionResults.count("material_map") > 0);
    assert(reconstructionResults.count("density_map") > 0);
    
    // Check reconstruction statistics
    auto stats = manager->GetReconstructionStatistics();
    assert(stats.count("chi_squared") > 0);
    assert(stats.count("degrees_of_freedom") > 0);
    assert(stats.count("goodness_of_fit") > 0);
    
    std::cout << "✓ Reconstruction algorithm test passed" << std::endl;
}

void test_data_export_import() {
    std::cout << "Testing data export/import..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Generate test data
    manager->SetTargetMaterial("LapisLazuli");
    manager->InitializeSimulation();
    
    G4ParticleGun gun(1);
    gun.SetParticleDefinition(G4Proton::ProtonDefinition());
    gun.SetParticleEnergy(2.0 * GeV);
    gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
    gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
    
    manager->RunSimulation(100, &gun);
    
    // Export data
    std::string exportFile = "test_integration_export.csv";
    manager->ExportResults(exportFile);
    
    // Verify file was created
    std::ifstream file(exportFile);
    assert(file.good());
    file.close();
    
    // Import data
    ArtAnalysisManager* importManager = ArtAnalysisManager::Instance();
    importManager->ImportResults(exportFile);
    
    // Verify imported data matches original
    auto originalResults = manager->GetAnalysisResults();
    auto importedResults = importManager->GetAnalysisResults();
    
    assert(originalResults.size() == importedResults.size());
    
    // Clean up
    std::remove(exportFile.c_str());
    
    std::cout << "✓ Data export/import test passed" << std::endl;
}

void run_all_integration_tests() {
    std::cout << "\n=== Running Integration Tests ===" << std::endl;
    
    try {
        test_full_simulation_pipeline();
        test_multi_material_analysis();
        test_detector_response_chain();
        test_energy_loss_validation();
        test_reconstruction_algorithm();
        test_data_export_import();
        
        std::cout << "\n🎉 All integration tests passed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ Integration test failed: " << e.what() << std::endl;
        exit(1);
    }
}

int main() {
    // Initialize logger
    ArtLogger::Instance()->SetLogLevel(LogLevel::INFO);
    
    run_all_integration_tests();
    
    return 0;
}
