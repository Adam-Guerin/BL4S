#include "ArtAnalysisManager.hh"
#include "G4SystemOfUnits.hh"

#include <cassert>
#include <cmath>
#include <iostream>

static void TestScatteringAngleStraight()
{
    const G4ThreeVector in(0.0, 0.0, 1.0);
    const G4ThreeVector out(0.0, 0.0, 1.0);
    const G4double theta = ArtAnalysisManager::ComputeScatteringAngle(in, out);
    assert(std::abs(theta) < 1e-12);
}

static void TestScatteringAngleKnownValue()
{
    const G4ThreeVector in(0.0, 0.0, 1.0);
    const G4ThreeVector out(1.0, 0.0, 1.0);
    const G4double theta = ArtAnalysisManager::ComputeScatteringAngle(in, out);
    assert(std::abs(theta - 45.0 * deg) < 1e-6);
}

static void TestInterceptAtZ()
{
    const G4ThreeVector p1(0.0, 0.0, -30.0 * cm);
    const G4ThreeVector p2(10.0 * mm, 0.0, -20.0 * cm);
    const G4ThreeVector atObject = ArtAnalysisManager::ComputeInterceptAtZ(p1, p2, 0.0);

    assert(std::abs(atObject.z()) < 1e-9);
    assert(atObject.x() > 0.0);
}

static void TestHighlandBudgetMonotonicity()
{
    const G4double kineticEnergy = 2.0 * GeV;
    const G4double smallTheta = 1.0 * mrad;
    const G4double largeTheta = 3.0 * mrad;

    const G4double smallBudget = ArtAnalysisManager::EstimateMaterialBudgetFromScatteringHighland(smallTheta, kineticEnergy);
    const G4double largeBudget = ArtAnalysisManager::EstimateMaterialBudgetFromScatteringHighland(largeTheta, kineticEnergy);

    assert(smallBudget > 0.0);
    assert(largeBudget > smallBudget);
}

static void TestTransmissionRatio()
{
    const G4double fullTransmission = ArtAnalysisManager::ComputeTransmission(100, 100);
    const G4double halfTransmission = ArtAnalysisManager::ComputeTransmission(100, 50);
    const G4double zeroIncident = ArtAnalysisManager::ComputeTransmission(0, 10);

    assert(std::abs(fullTransmission - 1.0) < 1e-12);
    assert(std::abs(halfTransmission - 0.5) < 1e-12);
    assert(std::abs(zeroIncident) < 1e-12);
}

static void TestProjectionAngleY()
{
    const G4ThreeVector straight(0.0, 0.0, 1.0);
    const G4ThreeVector tiltedY(0.0, 1.0, 1.0);

    const G4double a0 = ArtAnalysisManager::ComputeProjectionAngleY(straight);
    const G4double a1 = ArtAnalysisManager::ComputeProjectionAngleY(tiltedY);

    assert(std::abs(a0) < 1e-12);
    assert(std::abs(a1 - 45.0 * deg) < 1e-6);
}

static void TestThicknessFromEnergyLossAndStoppingPower()
{
    const G4double deltaE = 2.0 * MeV;
    const G4double stoppingPower = 0.02 * MeV / micrometer;
    const G4double thickness = ArtAnalysisManager::EstimateThicknessFromStoppingPower(deltaE, stoppingPower);
    assert(std::abs(thickness - 100.0 * micrometer) < 1e-9);
}

static void TestComputeEnergyLossFromCalorimeters()
{
    {
        const G4double loss = ArtAnalysisManager::ComputeEnergyLossFromCalorimeters(
            2.0 * GeV, true,
            1.95 * GeV, true,
            2.0 * GeV, 1.90 * GeV);
        assert(std::abs(loss - 50.0 * MeV) < 1e-9);
    }

    {
        const G4double loss = ArtAnalysisManager::ComputeEnergyLossFromCalorimeters(
            0.0, false,
            1.90 * GeV, true,
            2.0 * GeV, 1.85 * GeV);
        assert(std::abs(loss - 100.0 * MeV) < 1e-9);
    }

    {
        const G4double loss = ArtAnalysisManager::ComputeEnergyLossFromCalorimeters(
            0.0, false,
            0.0, false,
            2.0 * GeV, 1.80 * GeV);
        assert(std::abs(loss - 200.0 * MeV) < 1e-9);
    }
}

static void TestTrackerAngularResolutionModel()
{
    const G4double sigmaPos = 50.0 * micrometer;
    const G4double leverArm = 10.0 * cm;
    const G4double sigmaTheta = ArtAnalysisManager::ComputeTrackerAngularResolution(sigmaPos, leverArm);
    const G4double expected = std::sqrt(2.0) * sigmaPos / leverArm;
    assert(std::abs(sigmaTheta - expected) < 1e-12);
}

static void TestCalorimeterEnergyResolutionModel()
{
    const G4double energy = 2.0 * GeV;
    const G4double stochastic = 0.10;
    const G4double constant = 0.01;
    const G4double noise = 2.0 * MeV;

    const G4double sigma = ArtAnalysisManager::ComputeCalorimeterEnergySigma(
        energy, stochastic, constant, noise);

    const G4double termStochastic = stochastic / std::sqrt(energy / GeV);
    const G4double termNoise = noise / energy;
    const G4double expectedFrac = std::sqrt(termStochastic * termStochastic + constant * constant + termNoise * termNoise);
    const G4double expectedSigma = expectedFrac * energy;
    assert(std::abs(sigma - expectedSigma) < 1e-12);
}

static void TestProbabilityHelpers()
{
    assert(std::abs(ArtAnalysisManager::ClampProbability(-1.0) - 0.0) < 1e-12);
    assert(std::abs(ArtAnalysisManager::ClampProbability(2.0) - 1.0) < 1e-12);
    assert(std::abs(ArtAnalysisManager::ClampProbability(0.35) - 0.35) < 1e-12);

    assert(ArtAnalysisManager::ShouldAcceptByEfficiency(0.1, 0.9));
    assert(!ArtAnalysisManager::ShouldAcceptByEfficiency(0.95, 0.9));

    assert(ArtAnalysisManager::ShouldInjectFakeHit(0.05, 0.2));
    assert(!ArtAnalysisManager::ShouldInjectFakeHit(0.25, 0.2));
}

static void TestRadiationLengthFromThicknessAndBudget()
{
    const G4double x = 1.5 * mm;
    const G4double t = 0.25; // x/X0
    const G4double x0 = ArtAnalysisManager::EstimateRadiationLengthFromThicknessAndBudget(x, t);
    assert(std::abs(x0 - 6.0 * mm) < 1e-12);
}

static void TestLinearAttenuationFromTransmission()
{
    const G4double transmission = std::exp(-0.7 * 2.0); // mu=0.7/mm, x=2 mm
    const G4double mu = ArtAnalysisManager::EstimateLinearAttenuationFromTransmission(transmission, 2.0 * mm);
    assert(std::abs(mu - (0.7 / mm)) < 1e-9);
}

int main()
{
    TestScatteringAngleStraight();
    TestScatteringAngleKnownValue();
    TestInterceptAtZ();
    TestHighlandBudgetMonotonicity();
    TestTransmissionRatio();
    TestProjectionAngleY();
    TestThicknessFromEnergyLossAndStoppingPower();
    TestComputeEnergyLossFromCalorimeters();
    TestTrackerAngularResolutionModel();
    TestCalorimeterEnergyResolutionModel();
    TestProbabilityHelpers();
    TestRadiationLengthFromThicknessAndBudget();
    TestLinearAttenuationFromTransmission();
    std::cout << "bfs_ready_tests: OK" << std::endl;
    return 0;
}
