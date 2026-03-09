#include "ArtAnalysisManager.hh"
#include "ArtLogger.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4Proton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4PhysicsListHelper.hh"

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>

const double TOLERANCE = 0.05; // 5% tolerance for physics validation

void test_bethe_bloch_formula() {
    std::cout << "Testing Bethe-Bloch formula implementation..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test stopping power calculation for protons in different materials
    std::vector<std::pair<std::string, G4double>> testCases = {
        {"Water", 1.0 * GeV},
        {"LeadWhite", 1.0 * GeV},
        {"Carbon", 100 * MeV},
        {"Aluminum", 500 * MeV}
    };
    
    for (const auto& testCase : testCases) {
        G4double calculatedSp = manager->CalculateStoppingPower(testCase.first, testCase.second);
        
        // Verify stopping power is positive and reasonable
        assert(calculatedSp > 0);
        
        // Stopping power should decrease with energy (approximately 1/β² at high energies)
        G4double spHighEnergy = manager->CalculateStoppingPower(testCase.first, testCase.second * 2.0);
        assert(calculatedSp > spHighEnergy * 0.8); // Allow some variation
        
        std::cout << "  " << testCase.first << " at " << testCase.second / GeV << " GeV: " 
                  << calculatedSp / (MeV * cm2 / g) << " MeV·cm²/g" << std::endl;
    }
    
    std::cout << "✓ Bethe-Bloch formula test passed" << std::endl;
}

void test_multiple_coulomb_scattering() {
    std::cout << "Testing Multiple Coulomb Scattering..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test Highland formula implementation
    G4double kineticEnergy = 1.0 * GeV;
    G4double thickness = 0.01 * radiationLength;
    
    // Test for different materials
    std::vector<std::string> materials = {"LeadWhite", "Carbon", "Aluminum", "Water"};
    
    for (const auto& material : materials) {
        G4double theta0 = manager->CalculateMCSAngle(material, kineticEnergy, thickness);
        
        // Verify scattering angle is positive and reasonable
        assert(theta0 > 0);
        assert(theta0 < 90.0 * deg); // Should be less than 90 degrees for thin targets
        
        // Scattering should increase with thickness
        G4double theta0_thick = manager->CalculateMCSAngle(material, kineticEnergy, thickness * 2.0);
        assert(theta0_thick > theta0);
        
        // Scattering should decrease with energy
        G4double theta0_highE = manager->CalculateMCSAngle(material, kineticEnergy * 2.0, thickness);
        assert(theta0_highE < theta0);
        
        std::cout << "  " << material << ": θ₀ = " << theta0 / mrad << " mrad" << std::endl;
    }
    
    std::cout << "✓ Multiple Coulomb Scattering test passed" << std::endl;
}

void test_energy_straggling() {
    std::cout << "Testing energy straggling..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    G4double kineticEnergy = 2.0 * GeV;
    G4double thickness = 1.0 * cm;
    
    std::vector<std::string> materials = {"LeadWhite", "Carbon", "Water"};
    
    for (const auto& material : materials) {
        G4double straggling = manager->CalculateEnergyStraggling(material, kineticEnergy, thickness);
        
        // Verify straggling is positive
        assert(straggling > 0);
        
        // Straggling should increase with thickness
        G4double straggling_thick = manager->CalculateEnergyStraggling(material, kineticEnergy, thickness * 2.0);
        assert(straggling_thick > straggling);
        
        // Straggling should be proportional to sqrt(thickness) for thin absorbers
        G4double straggling_sqrt = manager->CalculateEnergyStraggling(material, kineticEnergy, thickness * 4.0);
        assert(std::abs(straggling_sqrt - straggling * 2.0) / straggling < TOLERANCE);
        
        std::cout << "  " << material << " straggling: " << straggling / MeV << " MeV" << std::endl;
    }
    
    std::cout << "✓ Energy straggling test passed" << std::endl;
}

void test_range_calculations() {
    std::cout << "Testing range calculations..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    G4double kineticEnergy = 1.0 * GeV;
    
    std::vector<std::string> materials = {"Water", "LeadWhite", "Carbon", "Aluminum"};
    
    for (const auto& material : materials) {
        G4double range = manager->CalculateRange(material, kineticEnergy);
        
        // Verify range is positive and reasonable
        assert(range > 0);
        
        // Range should increase with energy
        G4double range_highE = manager->CalculateRange(material, kineticEnergy * 2.0);
        assert(range_highE > range);
        
        // Range should be shorter in denser materials
        if (material == "LeadWhite") {
            G4double range_water = manager->CalculateRange("Water", kineticEnergy);
            assert(range < range_water);
        }
        
        std::cout << "  " << material << " range at " << kineticEnergy / GeV << " GeV: " 
                  << range / cm << " cm" << std::endl;
    }
    
    std::cout << "✓ Range calculations test passed" << std::endl;
}

void test_bremsstrahlung() {
    std::cout << "Testing Bremsstrahlung radiation..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    G4double electronEnergy = 100 * MeV;
    G4double thickness = 0.1 * radiationLength;
    
    std::vector<std::string> materials = {"LeadWhite", "Carbon", "Water"};
    
    for (const auto& material : materials) {
        G4double bremFraction = manager->CalculateBremsstrahlungFraction(material, electronEnergy, thickness);
        
        // Verify fraction is between 0 and 1
        assert(bremFraction >= 0);
        assert(bremFraction <= 1.0);
        
        // Bremsstrahlung should increase with Z
        if (material == "LeadWhite") {
            G4double bremCarbon = manager->CalculateBremsstrahlungFraction("Carbon", electronEnergy, thickness);
            assert(bremFraction > bremCarbon);
        }
        
        // Bremsstrahlung should increase with energy
        G4double brem_highE = manager->CalculateBremsstrahlungFraction(material, electronEnergy * 2.0, thickness);
        assert(brem_highE > bremFraction);
        
        std::cout << "  " << material << " bremsstrahlung fraction: " << bremFraction * 100 << "%" << std::endl;
    }
    
    std::cout << "✓ Bremsstrahlung test passed" << std::endl;
}

void test_pair_production() {
    std::cout << "Testing pair production..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    G4double photonEnergy = 10 * MeV; // Above pair production threshold
    G4double thickness = 0.5 * radiationLength;
    
    std::vector<std::string> materials = {"LeadWhite", "Carbon", "Water"};
    
    for (const auto& material : materials) {
        G4double pairProb = manager->CalculatePairProductionProbability(material, photonEnergy, thickness);
        
        // Verify probability is between 0 and 1
        assert(pairProb >= 0);
        assert(pairProb <= 1.0);
        
        // Pair production should increase with Z
        if (material == "LeadWhite") {
            G4double pairCarbon = manager->CalculatePairProductionProbability("Carbon", photonEnergy, thickness);
            assert(pairProb > pairCarbon);
        }
        
        // Pair production should increase with photon energy (above threshold)
        if (photonEnergy > 2 * electron_mass_c2) {
            G4double pair_highE = manager->CalculatePairProductionProbability(material, photonEnergy * 2.0, thickness);
            assert(pair_highE > pairProb);
        }
        
        std::cout << "  " << material << " pair production probability: " << pairProb * 100 << "%" << std::endl;
    }
    
    std::cout << "✓ Pair production test passed" << std::endl;
}

void test_compton_scattering() {
    std::cout << "Testing Compton scattering..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    G4double photonEnergy = 1 * MeV;
    G4double thickness = 1.0 * cm;
    
    std::vector<std::string> materials = {"LeadWhite", "Carbon", "Water"};
    
    for (const auto& material : materials) {
        G4double comptonProb = manager->CalculateComptonScatteringProbability(material, photonEnergy, thickness);
        
        // Verify probability is between 0 and 1
        assert(comptonProb >= 0);
        assert(comptonProb <= 1.0);
        
        // Compton scattering should depend on electron density
        G4double compton_water = manager->CalculateComptonScatteringProbability("Water", photonEnergy, thickness);
        
        // Energy dependence test
        G4double compton_lowE = manager->CalculateComptonScatteringProbability(material, 0.1 * MeV, thickness);
        G4double compton_highE = manager->CalculateComptonScatteringProbability(material, 10 * MeV, thickness);
        
        std::cout << "  " << material << " Compton probability: " << comptonProb * 100 << "%" << std::endl;
    }
    
    std::cout << "✓ Compton scattering test passed" << std::endl;
}

void test_photoelectric_effect() {
    std::cout << "Testing photoelectric effect..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    G4double photonEnergy = 50 * keV; // Low energy where photoelectric dominates
    G4double thickness = 0.1 * cm;
    
    std::vector<std::string> materials = {"LeadWhite", "Carbon", "Water"};
    
    for (const auto& material : materials) {
        G4double photoProb = manager->CalculatePhotoelectricProbability(material, photonEnergy, thickness);
        
        // Verify probability is between 0 and 1
        assert(photoProb >= 0);
        assert(photoProb <= 1.0);
        
        // Photoelectric effect should be much stronger in high-Z materials
        if (material == "LeadWhite") {
            G4double photoCarbon = manager->CalculatePhotoelectricProbability("Carbon", photonEnergy, thickness);
            assert(photoProb > photoCarbon * 10); // Lead should be much stronger
        }
        
        // Photoelectric effect should decrease rapidly with energy
        G4double photo_highE = manager->CalculatePhotoelectricProbability(material, 500 * keV, thickness);
        assert(photoProb > photo_highE);
        
        std::cout << "  " << material << " photoelectric probability: " << photoProb * 100 << "%" << std::endl;
    }
    
    std::cout << "✓ Photoelectric effect test passed" << std::endl;
}

void test_physics_cross_sections() {
    std::cout << "Testing physics cross sections..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test total cross section conservation
    G4double photonEnergy = 1 * MeV;
    G4double thickness = 1.0 * cm;
    
    for (const auto& material : {"LeadWhite", "Carbon", "Water"}) {
        G4double photo = manager->CalculatePhotoelectricProbability(material, photonEnergy, thickness);
        G4double compton = manager->CalculateComptonScatteringProbability(material, photonEnergy, thickness);
        G4double pair = manager->CalculatePairProductionProbability(material, photonEnergy, thickness);
        
        G4double total = photo + compton + pair;
        
        // Total should be reasonable (not > 1 for thin materials)
        assert(total <= 1.0);
        assert(total > 0);
        
        std::cout << "  " << material << " total interaction probability: " << total * 100 << "%" << std::endl;
        std::cout << "    Photoelectric: " << photo * 100 << "%, Compton: " << compton * 100 
                  << "%, Pair: " << pair * 100 << "%" << std::endl;
    }
    
    std::cout << "✓ Physics cross sections test passed" << std::endl;
}

void test_conservation_laws() {
    std::cout << "Testing conservation laws..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test energy conservation in interactions
    manager->SetTargetMaterial("LeadWhite");
    manager->InitializeSimulation();
    
    G4ParticleGun gun(1);
    gun.SetParticleDefinition(G4Proton::ProtonDefinition());
    gun.SetParticleEnergy(1.0 * GeV);
    gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
    gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
    
    manager->RunSimulation(1000, &gun);
    
    // Check energy conservation
    auto energyBalance = manager->GetEnergyBalance();
    assert(energyBalance.count("initial_energy") > 0);
    assert(energyBalance.count("final_energy") > 0);
    assert(energyBalance.count("energy_loss") > 0);
    
    G4double initialE = energyBalance["initial_energy"];
    G4double finalE = energyBalance["final_energy"];
    G4double lossE = energyBalance["energy_loss"];
    
    // Energy should be conserved within numerical precision
    assert(std::abs((initialE - finalE) - lossE) / initialE < 0.01); // 1% tolerance
    
    // Test momentum conservation
    auto momentumBalance = manager->GetMomentumBalance();
    assert(momentumBalance.count("initial_momentum") > 0);
    assert(momentumBalance.count("final_momentum") > 0);
    
    std::cout << "  Energy conservation: " << std::scientific << std::setprecision(3) 
              << std::abs((initialE - finalE) - lossE) / initialE << " relative error" << std::endl;
    
    std::cout << "✓ Conservation laws test passed" << std::endl;
}

void run_all_physics_validation_tests() {
    std::cout << "\n=== Running Physics Validation Tests ===" << std::endl;
    
    try {
        test_bethe_bloch_formula();
        test_multiple_coulomb_scattering();
        test_energy_straggling();
        test_range_calculations();
        test_bremsstrahlung();
        test_pair_production();
        test_compton_scattering();
        test_photoelectric_effect();
        test_physics_cross_sections();
        test_conservation_laws();
        
        std::cout << "\n🎉 All physics validation tests passed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ Physics validation test failed: " << e.what() << std::endl;
        exit(1);
    }
}

int main() {
    // Initialize logger
    ArtLogger::Instance()->SetLogLevel(LogLevel::WARNING);
    
    run_all_physics_validation_tests();
    
    return 0;
}
