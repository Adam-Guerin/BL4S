#include "ArtAnalysisManager.hh"
#include "ArtLogger.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4Proton.hh"

#include <chrono>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed_seconds() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000000.0;
    }
    
    double elapsed_milliseconds() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000.0;
    }
};

void test_simulation_speed() {
    std::cout << "Testing simulation speed..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    manager->SetTargetMaterial("LeadWhite");
    manager->InitializeSimulation();
    
    PerformanceTimer timer;
    
    // Test different event counts
    std::vector<int> eventCounts = {100, 500, 1000, 5000};
    
    for (int nEvents : eventCounts) {
        G4ParticleGun gun(1);
        gun.SetParticleDefinition(G4Proton::ProtonDefinition());
        gun.SetParticleEnergy(2.0 * GeV);
        gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
        gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
        
        timer.start();
        manager->RunSimulation(nEvents, &gun);
        double elapsed = timer.elapsed_seconds();
        
        double eventsPerSecond = nEvents / elapsed;
        std::cout << "  " << nEvents << " events: " << std::fixed << std::setprecision(2) 
                  << elapsed << "s (" << std::setprecision(0) << eventsPerSecond << " events/s)" << std::endl;
        
        // Performance should be reasonable
        assert(eventsPerSecond > 10); // At least 10 events per second
    }
    
    std::cout << "✓ Simulation speed test passed" << std::endl;
}

void test_memory_usage() {
    std::cout << "Testing memory usage..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    manager->SetTargetMaterial("Vermilion");
    manager->InitializeSimulation();
    
    // Test memory usage with increasing event counts
    std::vector<int> eventCounts = {1000, 5000, 10000, 20000};
    
    for (int nEvents : eventCounts) {
        // Clear previous data
        manager->ClearData();
        
        G4ParticleGun gun(1);
        gun.SetParticleDefinition(G4Proton::ProtonDefinition());
        gun.SetParticleEnergy(1.5 * GeV);
        gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
        gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
        
        manager->RunSimulation(nEvents, &gun);
        
        // Check memory usage (approximate through data size)
        size_t dataSize = manager->GetDataSize();
        double memoryPerEvent = static_cast<double>(dataSize) / nEvents;
        
        std::cout << "  " << nEvents << " events: " << dataSize << " bytes (" 
                  << std::fixed << std::setprecision(1) << memoryPerEvent << " bytes/event)" << std::endl;
        
        // Memory usage should be reasonable (less than 10KB per event)
        assert(memoryPerEvent < 10240);
    }
    
    std::cout << "✓ Memory usage test passed" << std::endl;
}

void test_reconstruction_performance() {
    std::cout << "Testing reconstruction performance..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    manager->SetTargetMaterial("LapisLazuli");
    manager->InitializeSimulation();
    
    // Generate data for reconstruction
    G4ParticleGun gun(1);
    gun.SetParticleDefinition(G4Proton::ProtonDefinition());
    gun.SetParticleEnergy(2.0 * GeV);
    gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
    gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
    
    manager->RunSimulation(10000, &gun);
    
    PerformanceTimer timer;
    
    // Test different reconstruction algorithms
    std::vector<std::string> algorithms = {"simple", "iterative", "bayesian"};
    
    for (const auto& algorithm : algorithms) {
        timer.start();
        auto results = manager->RunReconstruction(algorithm);
        double elapsed = timer.elapsed_seconds();
        
        std::cout << "  " << algorithm << " reconstruction: " << std::fixed << std::setprecision(3) 
                  << elapsed << "s" << std::endl;
        
        assert(!results.empty());
        
        // Reconstruction should complete in reasonable time
        assert(elapsed < 30.0); // Less than 30 seconds
    }
    
    std::cout << "✓ Reconstruction performance test passed" << std::endl;
}

void test_scalability() {
    std::cout << "Testing scalability..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Test with different material complexities
    std::vector<std::pair<std::string, int>> testCases = {
        {"Canvas", 1000},
        {"LeadWhite", 1000},
        {"Vermilion", 1000},
        {"LapisLazuli", 1000},
        {"MultiLayer", 1000}
    };
    
    for (const auto& testCase : testCases) {
        manager->SetTargetMaterial(testCase.first);
        manager->InitializeSimulation();
        
        G4ParticleGun gun(1);
        gun.SetParticleDefinition(G4Proton::ProtonDefinition());
        gun.SetParticleEnergy(2.0 * GeV);
        gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
        gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
        
        PerformanceTimer timer;
        timer.start();
        manager->RunSimulation(testCase.second, &gun);
        double elapsed = timer.elapsed_seconds();
        
        double eventsPerSecond = testCase.second / elapsed;
        std::cout << "  " << testCase.first << ": " << std::fixed << std::setprecision(2) 
                  << elapsed << "s (" << std::setprecision(0) << eventsPerSecond << " events/s)" << std::endl;
        
        // Performance should not degrade too much with complexity
        assert(eventsPerSecond > 5); // At least 5 events per second even for complex materials
    }
    
    std::cout << "✓ Scalability test passed" << std::endl;
}

void test_io_performance() {
    std::cout << "Testing I/O performance..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    manager->SetTargetMaterial("Malachite");
    manager->InitializeSimulation();
    
    // Generate test data
    G4ParticleGun gun(1);
    gun.SetParticleDefinition(G4Proton::ProtonDefinition());
    gun.SetParticleEnergy(1.8 * GeV);
    gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
    gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
    
    manager->RunSimulation(5000, &gun);
    
    PerformanceTimer timer;
    
    // Test export performance
    std::vector<std::string> formats = {"csv", "json", "root", "hdf5"};
    
    for (const auto& format : formats) {
        std::string filename = "test_perf_" + format + "_export";
        
        timer.start();
        manager->ExportResults(filename, format);
        double exportTime = timer.elapsed_seconds();
        
        // Test import performance
        timer.start();
        manager->ImportResults(filename, format);
        double importTime = timer.elapsed_seconds();
        
        std::cout << "  " << format << ": export " << std::fixed << std::setprecision(3) 
                  << exportTime << "s, import " << importTime << "s" << std::endl;
        
        // I/O should be reasonably fast
        assert(exportTime < 5.0);
        assert(importTime < 5.0);
        
        // Clean up
        std::remove((filename + "." + format).c_str());
    }
    
    std::cout << "✓ I/O performance test passed" << std::endl;
}

void benchmark_comparison() {
    std::cout << "Running benchmark comparison..." << std::endl;
    
    ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
    
    // Standard benchmark configuration
    manager->SetTargetMaterial("LeadWhite");
    manager->ConfigureDetector("tracker", 50.0 * micrometer, 10.0 * cm);
    manager->ConfigureDetector("calorimeter", 0.10, 0.01, 2.0 * MeV);
    manager->InitializeSimulation();
    
    G4ParticleGun gun(1);
    gun.SetParticleDefinition(G4Proton::ProtonDefinition());
    gun.SetParticleEnergy(2.0 * GeV);
    gun.SetParticlePosition(G4ThreeVector(0.0, 0.0, -30.0 * cm));
    gun.SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
    
    const int benchmarkEvents = 10000;
    
    PerformanceTimer timer;
    timer.start();
    manager->RunSimulation(benchmarkEvents, &gun);
    double simulationTime = timer.elapsed_seconds();
    
    timer.start();
    auto reconstructionResults = manager->RunReconstruction();
    double reconstructionTime = timer.elapsed_seconds();
    
    // Generate benchmark report
    std::ofstream report("benchmark_report.txt");
    report << "=== Art Analysis Simulation Benchmark ===" << std::endl;
    report << "Date: " << __DATE__ << " " << __TIME__ << std::endl;
    report << "Events: " << benchmarkEvents << std::endl;
    report << "Material: LeadWhite" << std::endl;
    report << "Beam Energy: 2.0 GeV protons" << std::endl;
    report << std::endl;
    report << "Performance Metrics:" << std::endl;
    report << "  Simulation Time: " << std::fixed << std::setprecision(3) << simulationTime << "s" << std::endl;
    report << "  Events/Second: " << std::setprecision(0) << benchmarkEvents / simulationTime << std::endl;
    report << "  Reconstruction Time: " << std::setprecision(3) << reconstructionTime << "s" << std::endl;
    report << "  Total Time: " << std::setprecision(3) << simulationTime + reconstructionTime << "s" << std::endl;
    report << std::endl;
    
    // Add system info
    report << "System Information:" << std::endl;
    report << "  Geant4 Version: " << G4Version << std::endl;
    report << "  Compiler: " << __VERSION__ << std::endl;
    
    report.close();
    
    std::cout << "  Benchmark report saved to benchmark_report.txt" << std::endl;
    std::cout << "✓ Benchmark comparison completed" << std::endl;
}

void run_all_performance_tests() {
    std::cout << "\n=== Running Performance Tests ===" << std::endl;
    
    try {
        test_simulation_speed();
        test_memory_usage();
        test_reconstruction_performance();
        test_scalability();
        test_io_performance();
        benchmark_comparison();
        
        std::cout << "\n🎉 All performance tests passed successfully!" << std::endl;
        std::cout << "📊 Check benchmark_report.txt for detailed performance metrics" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ Performance test failed: " << e.what() << std::endl;
        exit(1);
    }
}

int main() {
    // Initialize logger
    ArtLogger::Instance()->SetLogLevel(LogLevel::WARNING); // Reduce log overhead during performance tests
    
    run_all_performance_tests();
    
    return 0;
}
