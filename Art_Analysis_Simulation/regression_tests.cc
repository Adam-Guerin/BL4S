#include "ArtAnalysisManager.hh"
#include "ArtLogger.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4Proton.hh"

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>

// Reference values for regression testing (should be updated when algorithms change)
struct ReferenceValues {
    std::map<std::string, double> materialZ = {
        {"LeadWhite", 82.0},
        {"Vermilion", 80.0},
        {"LapisLazuli", 14.0},
        {"Malachite", 29.0},
        {"TinYellow", 50.0}
    };
    
    std::map<std::string, double> density = {
        {"LeadWhite", 6.5},
        {"Vermilion", 8.1},
        {"LapisLazuli", 2.4},
        {"Malachite", 4.0},
        {"TinYellow", 6.8}
    };
    
    // Reference stopping power values (MeV·cm²/g)
    std::map<std::pair<std::string, double>, double> stoppingPower = {
        {{"LeadWhite", 100.0}, 15.2},
        {{"Water", 100.0}, 2.8},
        {{"Carbon", 100.0}, 2.1},
        {{"LeadWhite", 1000.0}, 4.8},
        {{"Water", 1000.0}, 1.2}
    };
    
    // Reference scattering angles (mrad)
    std::map<std::tuple<std::string, double, double>, double> scatteringAngles = {
        {{"LeadWhite", 1000.0, 0.01}, 12.5},
        {{"Water", 1000.0, 0.01}, 3.2},
        {{"Carbon", 1000.0, 0.01}, 2.8}
    };
};

const double REGRESSION_TOLERANCE = 0.02; // 2% tolerance for regression tests

void test_material_z_regression() {
    std::cout << "Testing material Z calculation regression..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    ReferenceValues ref;
    
    for (const auto& [material, expectedZ] : ref.materialZ) {
        double calculatedZ = manager->CalculateMaterialZ(material);
        double relativeError = std::abs(calculatedZ - expectedZ) / expectedZ;
        
        std::cout << "  " << material << ": expected " << expectedZ 
                  << ", got " << calculatedZ << " (error: " << relativeError * 100 << "%)" << std::endl;
        
        assert(relativeError < REGRESSION_TOLERANCE);
    }
    
    std::cout << "✓ Material Z regression test passed" << std::endl;
}

void test_density_regression() {
    std::cout << "Testing density calculation regression..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    ReferenceValues ref;
    
    for (const auto& [material, expectedDensity] : ref.density) {
        double calculatedDensity = manager->CalculateDensity(material) / (g/cm3);
        double relativeError = std::abs(calculatedDensity - expectedDensity) / expectedDensity;
        
        std::cout << "  " << material << ": expected " << expectedDensity 
                  << " g/cm³, got " << calculatedDensity << " g/cm³ (error: " << relativeError * 100 << "%)" << std::endl;
        
        assert(relativeError < REGRESSION_TOLERANCE);
    }
    
    std::cout << "✓ Density regression test passed" << std::endl;
}

void test_stopping_power_regression() {
    std::cout << "Testing stopping power regression..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    ReferenceValues ref;
    
    for (const auto& [key, expectedSp] : ref.stoppingPower) {
        const auto& [material, energy] = key;
        double calculatedSp = manager->CalculateStoppingPower(material, energy * MeV) / (MeV * cm2 / g);
        double relativeError = std::abs(calculatedSp - expectedSp) / expectedSp;
        
        std::cout << "  " << material << " at " << energy << " MeV: expected " << expectedSp 
                  << " MeV·cm²/g, got " << calculatedSp << " MeV·cm²/g (error: " << relativeError * 100 << "%)" << std::endl;
        
        assert(relativeError < REGRESSION_TOLERANCE);
    }
    
    std::cout << "✓ Stopping power regression test passed" << std::endl;
}

void test_scattering_angle_regression() {
    std::cout << "Testing scattering angle regression..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    ReferenceValues ref;
    
    for (const auto& [key, expectedAngle] : ref.scatteringAngles) {
        const auto& [material, energy, thickness] = key;
        double calculatedAngle = manager->CalculateMCSAngle(material, energy * MeV, thickness * radiationLength) / mrad;
        double relativeError = std::abs(calculatedAngle - expectedAngle) / expectedAngle;
        
        std::cout << "  " << material << " at " << energy << " MeV, " << thickness << " X₀: expected " << expectedAngle 
                  << " mrad, got " << calculatedAngle << " mrad (error: " << relativeError * 100 << "%)" << std::endl;
        
        assert(relativeError < REGRESSION_TOLERANCE);
    }
    
    std::cout << "✓ Scattering angle regression test passed" << std::endl;
}

void test_thickness_estimation_regression() {
    std::cout << "Testing thickness estimation regression..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test cases with known inputs and expected outputs
    std::vector<std::tuple<G4double, G4double, G4double>> testCases = {
        {50.0 * keV, 82.0, 10.0 * micrometer},    // Lead, 50 keV loss
        {100.0 * keV, 82.0, 25.0 * micrometer},   // Lead, 100 keV loss
        {50.0 * keV, 14.0, 35.0 * micrometer},    // Silicon, 50 keV loss
        {100.0 * keV, 14.0, 85.0 * micrometer}    // Silicon, 100 keV loss
    };
    
    for (const auto& [energyLoss, Z, expectedThickness] : testCases) {
        double calculatedThickness = manager->EstimateThickness(energyLoss, Z);
        double relativeError = std::abs(calculatedThickness - expectedThickness) / expectedThickness;
        
        std::cout << "  Z=" << Z << ", ΔE=" << energyLoss / keV << " keV: expected " << expectedThickness / micrometer 
                  << " μm, got " << calculatedThickness / micrometer << " μm (error: " << relativeError * 100 << "%)" << std::endl;
        
        assert(relativeError < REGRESSION_TOLERANCE * 2); // Allow more tolerance for thickness estimation
    }
    
    std::cout << "✓ Thickness estimation regression test passed" << std::endl;
}

void test_transmission_calculation_regression() {
    std::cout << "Testing transmission calculation regression..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test cases with known inputs and expected outputs
    std::vector<std::tuple<int, int, double>> testCases = {
        {100, 100, 1.0},
        {100, 50, 0.5},
        {1000, 750, 0.75},
        {500, 0, 0.0},
        {0, 100, 0.0}
    };
    
    for (const auto& [incident, transmitted, expected] : testCases) {
        double calculated = manager->ComputeTransmission(incident, transmitted);
        double absoluteError = std::abs(calculated - expected);
        
        std::cout << "  " << incident << "→" << transmitted << ": expected " << expected 
                  << ", got " << calculated << " (error: " << absoluteError << ")" << std::endl;
        
        assert(absoluteError < 1e-12);
    }
    
    std::cout << "✓ Transmission calculation regression test passed" << std::endl;
}

void test_energy_loss_computation_regression() {
    std::cout << "Testing energy loss computation regression..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test calorimeter energy loss computation
    std::vector<std::tuple<G4double, bool, G4double, bool, G4double, G4double, G4double>> testCases = {
        {2.0 * GeV, true, 1.95 * GeV, true, 2.0 * GeV, 1.90 * GeV, 50.0 * MeV},
        {0.0, false, 1.90 * GeV, true, 2.0 * GeV, 1.85 * GeV, 100.0 * MeV},
        {0.0, false, 0.0, false, 2.0 * GeV, 1.80 * GeV, 200.0 * MeV}
    };
    
    for (const auto& [preCal, preCalValid, postCal, postCalValid, preTracker, postTracker, expected] : testCases) {
        double calculated = manager->ComputeEnergyLossFromCalorimeters(
            preCal, preCalValid, postCal, postCalValid, preTracker, postTracker);
        double absoluteError = std::abs(calculated - expected);
        
        std::cout << "  Calorimeter test: expected " << expected / MeV 
                  << " MeV, got " << calculated / MeV << " MeV (error: " << absoluteError / MeV << " MeV)" << std::endl;
        
        assert(absoluteError < 1e-9 * MeV);
    }
    
    std::cout << "✓ Energy loss computation regression test passed" << std::endl;
}

void test_angular_resolution_regression() {
    std::cout << "Testing angular resolution regression..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test tracker angular resolution
    G4double sigmaPos = 50.0 * micrometer;
    G4double leverArm = 10.0 * cm;
    G4double expected = std::sqrt(2.0) * sigmaPos / leverArm;
    
    G4double calculated = manager->ComputeTrackerAngularResolution(sigmaPos, leverArm);
    double relativeError = std::abs(calculated - expected) / expected;
    
    std::cout << "  Tracker resolution: expected " << expected / rad 
              << " rad, got " << calculated / rad << " rad (error: " << relativeError * 100 << "%)" << std::endl;
    
    assert(relativeError < 1e-12);
    
    // Test calorimeter energy resolution
    G4double energy = 2.0 * GeV;
    G4double stochastic = 0.10;
    G4double constant = 0.01;
    G4double noise = 2.0 * MeV;
    
    G4double termStochastic = stochastic / std::sqrt(energy / GeV);
    G4double termNoise = noise / energy;
    G4double expectedFrac = std::sqrt(termStochastic * termStochastic + constant * constant + termNoise * termNoise);
    G4double expectedSigma = expectedFrac * energy;
    
    G4double calculatedSigma = manager->ComputeCalorimeterEnergySigma(energy, stochastic, constant, noise);
    relativeError = std::abs(calculatedSigma - expectedSigma) / expectedSigma;
    
    std::cout << "  Calorimeter resolution: expected " << expectedSigma / MeV 
              << " MeV, got " << calculatedSigma / MeV << " MeV (error: " << relativeError * 100 << "%)" << std::endl;
    
    assert(relativeError < 1e-12);
    
    std::cout << "✓ Angular resolution regression test passed" << std::endl;
}

void test_probability_helpers_regression() {
    std::cout << "Testing probability helper functions regression..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test probability clamping
    assert(std::abs(manager->ClampProbability(-1.0) - 0.0) < 1e-12);
    assert(std::abs(manager->ClampProbability(2.0) - 1.0) < 1e-12);
    assert(std::abs(manager->ClampProbability(0.35) - 0.35) < 1e-12);
    
    // Test efficiency acceptance
    assert(manager->ShouldAcceptByEfficiency(0.1, 0.9));
    assert(!manager->ShouldAcceptByEfficiency(0.95, 0.9));
    
    // Test fake hit injection
    assert(manager->ShouldInjectFakeHit(0.05, 0.2));
    assert(!manager->ShouldInjectFakeHit(0.25, 0.2));
    
    std::cout << "✓ Probability helper functions regression test passed" << std::endl;
}

void test_reconstruction_algorithm_regression() {
    std::cout << "Testing reconstruction algorithm regression..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Setup test case
    manager->SetTargetMaterial("LeadWhite");
    manager->InitializeSimulation();
    
    G4ParticleGun gun(1);
    gun.SetParticleDefinition(G4Proton::ProtonDefinition());
    gun.SetParticleEnergy(2.0 * GeV);
    gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
    gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
    
    manager->RunSimulation(1000, &gun);
    
    // Test reconstruction with different algorithms
    std::vector<std::string> algorithms = {"simple", "iterative"};
    
    for (const auto& algorithm : algorithms) {
        auto results = manager->RunReconstruction(algorithm);
        
        // Verify expected outputs are present
        assert(results.count("thickness_map") > 0);
        assert(results.count("material_map") > 0);
        assert(results.count("density_map") > 0);
        assert(results.count("chi_squared") > 0);
        
        // Verify quality metrics are reasonable
        double chi2 = results["chi_squared"];
        assert(chi2 > 0);
        assert(chi2 < 100); // Should not be too large for good reconstruction
        
        std::cout << "  " << algorithm << " reconstruction: χ² = " << chi2 << std::endl;
    }
    
    std::cout << "✓ Reconstruction algorithm regression test passed" << std::endl;
}

void create_regression_baseline() {
    std::cout << "Creating regression baseline..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    ReferenceValues ref;
    
    // Generate current values for baseline
    std::ofstream baseline("regression_baseline.txt");
    baseline << "# Regression baseline for Art Analysis Simulation" << std::endl;
    baseline << "# Generated on: " << __DATE__ << " " << __TIME__ << std::endl;
    baseline << std::endl;
    
    baseline << "[Material_Z]" << std::endl;
    for (const auto& [material, expectedZ] : ref.materialZ) {
        double currentZ = manager->CalculateMaterialZ(material);
        baseline << material << " = " << currentZ << std::endl;
    }
    baseline << std::endl;
    
    baseline << "[Density_g_cm3]" << std::endl;
    for (const auto& [material, expectedDensity] : ref.density) {
        double currentDensity = manager->CalculateDensity(material) / (g/cm3);
        baseline << material << " = " << currentDensity << std::endl;
    }
    baseline << std::endl;
    
    baseline << "[Stopping_Power_MeV_cm2_g]" << std::endl;
    for (const auto& [key, expectedSp] : ref.stoppingPower) {
        const auto& [material, energy] = key;
        double currentSp = manager->CalculateStoppingPower(material, energy * MeV) / (MeV * cm2 / g);
        baseline << material << "_" << energy << "MeV = " << currentSp << std::endl;
    }
    baseline << std::endl;
    
    baseline.close();
    
    std::cout << "  Regression baseline saved to regression_baseline.txt" << std::endl;
}

void run_all_regression_tests() {
    std::cout << "\n=== Running Regression Tests ===" << std::endl;
    
    try {
        test_material_z_regression();
        test_density_regression();
        test_stopping_power_regression();
        test_scattering_angle_regression();
        test_thickness_estimation_regression();
        test_transmission_calculation_regression();
        test_energy_loss_computation_regression();
        test_angular_resolution_regression();
        test_probability_helpers_regression();
        test_reconstruction_algorithm_regression();
        
        std::cout << "\n🎉 All regression tests passed successfully!" << std::endl;
        std::cout << "💡 Run with --create-baseline to update reference values" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ Regression test failed: " << e.what() << std::endl;
        std::cout << "💡 Consider updating baseline values if algorithm changes are intentional" << std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    // Initialize logger
    ArtLogger::Instance()->SetLogLevel(LogLevel::WARNING);
    
    // Check for baseline creation flag
    if (argc > 1 && std::string(argv[1]) == "--create-baseline") {
        create_regression_baseline();
        return 0;
    }
    
    run_all_regression_tests();
    
    return 0;
}
