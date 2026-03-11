#include "ArtAnalysisManager.hh"
#include "ArtException.hh"
#include "ArtLogger.hh"
#include "G4EmCalculator.hh"
#include "G4Material.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

ArtAnalysisManager* ArtAnalysisManager::fInstance = nullptr;

ArtAnalysisManager* ArtAnalysisManager::Instance()
{
    if (!fInstance) {
        fInstance = new ArtAnalysisManager();
    }
    return fInstance;
}

ArtAnalysisManager::ArtAnalysisManager()
    : fGridNX(20),
      fGridNY(20),
      fGridHalfWidth(10.0 * cm),
      fGridHalfHeight(10.0 * cm)
{
    ConfigureGrid(fGridNX, fGridNY, fGridHalfWidth, fGridHalfHeight);
    ART_INFO("ArtAnalysisManager initialized");
}

ArtAnalysisManager::~ArtAnalysisManager()
{
    ART_INFO("ArtAnalysisManager destroyed");
}

void ArtAnalysisManager::AddElectronTrack(const ElectronTrack& track)
{
    fElectronTracks.push_back(track);
}

void ArtAnalysisManager::RecordEnergyDeposit(const G4String& material, G4double energy)
{
    if (energy <= 0.0) {
        return;
    }
    fEnergyDepositsByMaterial[material].push_back(energy);
}

void ArtAnalysisManager::RecordScattering(const G4String& material, G4double angle)
{
    if (angle < 0.0) {
        return;
    }
    fScatteringAnglesByMaterial[material].push_back(angle);
}

void ArtAnalysisManager::ConfigureGrid(G4int nX, G4int nY, G4double halfWidth, G4double halfHeight)
{
    fGridNX = std::max(1, nX);
    fGridNY = std::max(1, nY);
    fGridHalfWidth = std::max(1.0 * mm, halfWidth);
    fGridHalfHeight = std::max(1.0 * mm, halfHeight);

    const size_t nCells = static_cast<size_t>(fGridNX * fGridNY);
    fCellCounts.assign(nCells, 0);
    fCellIncidentCounts.assign(nCells, 0);
    fCellTransmittedCounts.assign(nCells, 0);
    fCellScatteringSum.assign(nCells, 0.0);
    fCellEnergyLossSum.assign(nCells, 0.0);
    fCellThicknessSum.assign(nCells, 0.0);
    fCellIncidentEnergySum.assign(nCells, 0.0);
    fCellTransmittedEnergySum.assign(nCells, 0.0);
}

void ArtAnalysisManager::AddTomographyEvent(const TomographyEvent& eventRecord)
{
    TomographyEvent enriched = eventRecord;
    enriched.estimatedThickness = 0.0;
    enriched.estimatedRadiationLength = 0.0;
    enriched.estimatedStoppingPower = 0.0;
    enriched.estimatedMu = 0.0;
    enriched.estimatedTransmission = 0.0;
    enriched.inferredMaterial = "Unknown";

    if (enriched.estimatedXOverX0 > 0.0 && enriched.energyLoss > 0.0 && enriched.initialEnergy > 0.0) {
        G4String materialName;
        G4double thickness = 0.0;
        G4double radiationLength = 0.0;
        G4double stoppingPower = 0.0;
        if (InferMaterialAndThicknessFromObservables(
                enriched.estimatedXOverX0,
                enriched.energyLoss,
                enriched.initialEnergy,
                materialName,
                thickness,
                radiationLength,
                stoppingPower)) {
            enriched.inferredMaterial = materialName;
            enriched.estimatedThickness = thickness;
            enriched.estimatedRadiationLength = radiationLength;
            enriched.estimatedStoppingPower = stoppingPower;
            const G4double safeInitial = std::max(1e-9 * MeV, enriched.initialEnergy);
            enriched.estimatedMu = stoppingPower / safeInitial;
            enriched.estimatedTransmission = std::exp(-enriched.estimatedMu * thickness);
        }
    }

    fTomographyEvents.push_back(enriched);

    if (!enriched.hasIncomingTrack || !enriched.hasOutgoingTrack) {
        return;
    }

    const G4int cellIndex = ComputeCellIndex(enriched.objectIntersection);
    if (cellIndex < 0) {
        return;
    }

    const size_t index = static_cast<size_t>(cellIndex);
    fCellCounts[index] += 1;
    fCellScatteringSum[index] += enriched.scatteringAngle;
    fCellEnergyLossSum[index] += enriched.energyLoss;
    fCellThicknessSum[index] += enriched.estimatedThickness;
    fCellIncidentCounts[index] += 1;
    fCellIncidentEnergySum[index] += std::max(0.0, enriched.initialEnergy);
    if (enriched.transmitted) {
        fCellTransmittedCounts[index] += 1;
        fCellTransmittedEnergySum[index] += std::max(0.0, enriched.downstreamCalorimeterEnergy);
    }
}

void ArtAnalysisManager::AnalyzeLayers()
{
    fLayerAnalysis.clear();

    if (fEnergyDepositsByMaterial.empty()) {
        throw AnalysisException("No energy deposit data available for analysis");
    }

    G4double referenceKineticEnergy = 2.0 * GeV;
    G4double sumInitial = 0.0;
    G4int nInitial = 0;
    for (const auto& eventRecord : fTomographyEvents) {
        if (eventRecord.initialEnergy > 0.0) {
            sumInitial += eventRecord.initialEnergy;
            nInitial++;
        }
    }
    if (nInitial > 0) {
        referenceKineticEnergy = sumInitial / nInitial;
    }

    for (const auto& entry : fEnergyDepositsByMaterial) {
        LayerAnalysis layer{};
        layer.materialName = entry.first;
        layer.meanEnergyDeposit = CalculateMean(entry.second);

        auto scatterIt = fScatteringAnglesByMaterial.find(entry.first);
        layer.meanScatteringAngle =
            (scatterIt != fScatteringAnglesByMaterial.end()) ? CalculateMean(scatterIt->second) : 0.0;

        layer.atomicNumber = CalculateMaterialZ(entry.first);
        layer.inferredThickness = EstimateThickness(layer.materialName, layer.meanEnergyDeposit, referenceKineticEnergy);
        layer.density = CalculateDensity(entry.first);
        layer.isHistoricalPigment = IsHistoricalPigment(entry.first);
        layer.sampleCount = static_cast<G4int>(entry.second.size());

        fLayerAnalysis.push_back(layer);
    }
}

void ArtAnalysisManager::GenerateReport()
{
    std::cout << "\n=== BFS-READY ELECTRON TOMOGRAPHY REPORT ===" << std::endl;
    std::cout << "Events recorded: " << fTomographyEvents.size() << std::endl;

    G4int incidentCount = 0;
    G4int transmittedCount = 0;
    G4int validTomographyEvents = 0;
    G4double scatteringSum = 0.0;
    G4double energyLossSum = 0.0;
    G4double xOverX0Sum = 0.0;
    for (const auto& eventRecord : fTomographyEvents) {
        if (eventRecord.hasTrigger) {
            incidentCount++;
        }
        if (eventRecord.transmitted) {
            transmittedCount++;
        }
        if (!eventRecord.hasIncomingTrack || !eventRecord.hasOutgoingTrack || !eventRecord.hasCalorimeterEnergy) {
            continue;
        }
        validTomographyEvents++;
        scatteringSum += eventRecord.scatteringAngle;
        energyLossSum += eventRecord.energyLoss;
        xOverX0Sum += eventRecord.estimatedXOverX0;
    }

    std::cout << "Transmission: " << ComputeTransmission(incidentCount, transmittedCount) << std::endl;
    std::cout << "Valid tomography events: " << validTomographyEvents << std::endl;
    if (validTomographyEvents > 0) {
        std::cout << "Mean scattering angle: " << (scatteringSum / validTomographyEvents) / mrad << " mrad" << std::endl;
        std::cout << "Mean energy loss: " << (energyLossSum / validTomographyEvents) / MeV << " MeV" << std::endl;
        std::cout << "Mean estimated x/X0 (Highland): " << (xOverX0Sum / validTomographyEvents) << std::endl;
    }

    if (!fLayerAnalysis.empty()) {
        std::cout << "\n--- Material interaction summary ---" << std::endl;
        for (const auto& layer : fLayerAnalysis) {
            std::cout << "Material: " << layer.materialName
                      << " | mean dE(step): " << layer.meanEnergyDeposit / keV << " keV"
                      << " | inferred thickness: " << layer.inferredThickness / micrometer << " um"
                      << " | mean scatter proxy: " << layer.meanScatteringAngle / mrad << " mrad"
                      << " | samples: " << layer.sampleCount
                      << std::endl;
        }
    }

    std::cout << "\nGrid: " << fGridNX << "x" << fGridNY
              << " over " << (2.0 * fGridHalfWidth) / cm << " x "
              << (2.0 * fGridHalfHeight) / cm << " cm^2" << std::endl;
}

void ArtAnalysisManager::SaveResults(const G4String& filename)
{
    std::ofstream summaryFile(filename);
    if (!summaryFile.is_open()) {
        throw AnalysisException("Cannot open file: " + filename);
    }

    summaryFile << "# BFS-ready summary\n";
    summaryFile << "event_id,has_trigger,has_cal_in,has_cal_out,valid_tomography,transmitted,transmitted_weight,projection_angle_y_mrad,object_shift_x_mm,object_shift_y_mm,object_tilt_y_mrad,theta_mrad,x_over_X0_est,thickness_est_mm,X0_est_mm,dedx_est_MeV_per_mm,mu_est_per_mm,transmission_est,inferred_material,initial_energy_MeV,cal_in_energy_MeV,cal_out_energy_MeV,final_energy_MeV,deltaE_MeV,obj_x_mm,obj_y_mm,in_ux,in_uy,in_uz,out_ux,out_uy,out_uz\n";
    for (const auto& eventRecord : fTomographyEvents) {
        const bool valid = eventRecord.hasIncomingTrack && eventRecord.hasOutgoingTrack && eventRecord.hasCalorimeterEnergy;
        summaryFile << eventRecord.eventID << ","
                    << (eventRecord.hasTrigger ? 1 : 0) << ","
                    << (eventRecord.hasCalorimeterIn ? 1 : 0) << ","
                    << (eventRecord.hasCalorimeterOut ? 1 : 0) << ","
                    << (valid ? 1 : 0) << ","
                    << (eventRecord.transmitted ? 1 : 0) << ","
                    << eventRecord.transmittedWeight << ","
                    << eventRecord.projectionAngleY / mrad << ","
                    << eventRecord.objectShiftX / mm << ","
                    << eventRecord.objectShiftY / mm << ","
                    << eventRecord.objectTiltY / mrad << ","
                    << eventRecord.scatteringAngle / mrad << ","
                    << eventRecord.estimatedXOverX0 << ","
                    << eventRecord.estimatedThickness / mm << ","
                    << eventRecord.estimatedRadiationLength / mm << ","
                    << eventRecord.estimatedStoppingPower / (MeV / mm) << ","
                    << eventRecord.estimatedMu / (1.0 / mm) << ","
                    << eventRecord.estimatedTransmission << ","
                    << eventRecord.inferredMaterial << ","
                    << eventRecord.initialEnergy / MeV << ","
                    << eventRecord.upstreamCalorimeterEnergy / MeV << ","
                    << eventRecord.downstreamCalorimeterEnergy / MeV << ","
                    << eventRecord.finalEnergy / MeV << ","
                    << eventRecord.energyLoss / MeV << ","
                    << eventRecord.objectIntersection.x() / mm << ","
                    << eventRecord.objectIntersection.y() / mm << ","
                    << eventRecord.incomingDirection.x() << ","
                    << eventRecord.incomingDirection.y() << ","
                    << eventRecord.incomingDirection.z() << ","
                    << eventRecord.outgoingDirection.x() << ","
                    << eventRecord.outgoingDirection.y() << ","
                    << eventRecord.outgoingDirection.z()
                    << "\n";
    }

    summaryFile << "\n# grid_cell,ix,iy,count,mean_theta_mrad,mean_deltaE_MeV,mean_thickness_mm,transmission,transmission_energy,mu_est_per_mm\n";
    for (G4int iy = 0; iy < fGridNY; ++iy) {
        for (G4int ix = 0; ix < fGridNX; ++ix) {
            const G4int linear = iy * fGridNX + ix;
            const size_t index = static_cast<size_t>(linear);
            const G4int count = fCellCounts[index];
            const G4double meanTheta = (count > 0) ? fCellScatteringSum[index] / count : 0.0;
            const G4double meanDeltaE = (count > 0) ? fCellEnergyLossSum[index] / count : 0.0;
            const G4double meanThickness = (count > 0) ? fCellThicknessSum[index] / count : 0.0;
            const G4double transmission = ComputeTransmission(fCellIncidentCounts[index], fCellTransmittedCounts[index]);
            const G4double transmissionEnergy = (fCellIncidentEnergySum[index] > 0.0)
                ? std::clamp(fCellTransmittedEnergySum[index] / fCellIncidentEnergySum[index], 0.0, 1.0)
                : 0.0;
            const G4double mu = EstimateLinearAttenuationFromTransmission(transmission, meanThickness);
            summaryFile << linear << ","
                        << ix << ","
                        << iy << ","
                        << count << ","
                        << meanTheta / mrad << ","
                        << meanDeltaE / MeV << ","
                        << meanThickness / mm << ","
                        << transmission << ","
                        << transmissionEnergy << ","
                        << mu / (1.0 / mm)
                        << "\n";
        }
    }
}

G4double ArtAnalysisManager::CalculateMean(const std::vector<G4double>& values)
{
    if (values.empty()) {
        return 0.0;
    }

    G4double sum = 0.0;
    for (const G4double value : values) {
        sum += value;
    }
    return sum / values.size();
}

G4double ArtAnalysisManager::CalculateStdDev(const std::vector<G4double>& values)
{
    if (values.size() < 2) {
        return 0.0;
    }

    const G4double mean = CalculateMean(values);
    G4double squared = 0.0;
    for (const G4double value : values) {
        const G4double diff = value - mean;
        squared += diff * diff;
    }
    return std::sqrt(squared / (values.size() - 1));
}

G4double ArtAnalysisManager::CalculateMaterialZ(const G4String& material) const
{
    if (material.find("Lead") != G4String::npos || material.find("Pb") != G4String::npos) return 82.0;
    if (material.find("Vermilion") != G4String::npos || material.find("Hg") != G4String::npos) return 80.0;
    if (material.find("Tin") != G4String::npos || material.find("Sn") != G4String::npos) return 50.0;
    if (material.find("Malachite") != G4String::npos || material.find("Cu") != G4String::npos) return 29.0;
    if (material.find("Titan") != G4String::npos || material.find("Ti") != G4String::npos) return 22.0;
    if (material.find("Silicon") != G4String::npos || material.find("G4_Si") != G4String::npos) return 14.0;
    if (material.find("G4_AIR") != G4String::npos) return 7.3;
    return 10.0;
}

G4double ArtAnalysisManager::EstimateThickness(const G4String& material, G4double energyLoss, G4double kineticEnergy)
{
    if (energyLoss <= 0.0 || kineticEnergy <= 0.0) {
        return 0.0;
    }

    const G4double stoppingPower = CalculateStoppingPower(material, kineticEnergy);
    return EstimateThicknessFromStoppingPower(energyLoss, stoppingPower);
}

void ArtAnalysisManager::Reset()
{
    fElectronTracks.clear();
    fTomographyEvents.clear();
    fEnergyDepositsByMaterial.clear();
    fScatteringAnglesByMaterial.clear();
    fLayerAnalysis.clear();
    ConfigureGrid(fGridNX, fGridNY, fGridHalfWidth, fGridHalfHeight);
}

void ArtAnalysisManager::AnalyzeThicknessByEnergyLoss()
{
    ART_INFO("AnalyzeThicknessByEnergyLoss is covered by AnalyzeLayers");
}

void ArtAnalysisManager::DetectHistoricalPigments()
{
    ART_INFO("DetectHistoricalPigments is covered by layer flags");
}

void ArtAnalysisManager::CalculateScatteringSignatures()
{
    ART_INFO("CalculateScatteringSignatures is covered by tomography event data");
}

G4double ArtAnalysisManager::CalculateStoppingPower(const G4String& material, G4double energy) const
{
    if (energy <= 0.0) {
        return 0.0;
    }

    const G4ParticleDefinition* electron = G4ParticleTable::GetParticleTable()->FindParticle("e-");
    G4Material* mat = G4Material::GetMaterial(material, false);
    if (electron != nullptr && mat != nullptr) {
        G4EmCalculator emCalc;
        const G4double dedx = emCalc.ComputeTotalDEDX(energy, electron, mat);
        if (dedx > 0.0) {
            return dedx;
        }
    }

    // Fallback parametric model
    const G4double Z = CalculateMaterialZ(material);
    const G4double density = CalculateDensity(material);
    const G4double beta2 = std::max(1e-8, 1.0 - std::pow((511.0 * keV) / (energy + 511.0 * keV), 2.0));
    const G4double I = std::max(50.0 * eV, 16.0 * Z * eV);
    const G4double argument = std::max(1.0 + 1e-8, (2.0 * 511.0 * keV * beta2) / I);
    return std::max(0.1 * keV / micrometer, 0.1536 * MeV * cm2 / g * density * std::log(argument));
}

G4bool ArtAnalysisManager::IsHistoricalPigment(const G4String& material)
{
    return material.find("Lead") != G4String::npos ||
           material.find("Vermilion") != G4String::npos ||
           material.find("Lapis") != G4String::npos ||
           material.find("Malachite") != G4String::npos ||
           material.find("TinYellow") != G4String::npos;
}

G4double ArtAnalysisManager::CalculateDensity(const G4String& material) const
{
    if (material.find("Lead") != G4String::npos) return 6.5 * g / cm3;
    if (material.find("Vermilion") != G4String::npos) return 8.1 * g / cm3;
    if (material.find("Lapis") != G4String::npos) return 2.4 * g / cm3;
    if (material.find("Malachite") != G4String::npos) return 4.0 * g / cm3;
    if (material.find("TinYellow") != G4String::npos) return 6.8 * g / cm3;
    if (material.find("G4_AIR") != G4String::npos) return 0.0012 * g / cm3;
    if (material.find("G4_Si") != G4String::npos) return 2.33 * g / cm3;
    return 2.0 * g / cm3;
}

G4double ArtAnalysisManager::ComputeScatteringAngle(const G4ThreeVector& incoming, const G4ThreeVector& outgoing)
{
    if (incoming.mag2() <= 0.0 || outgoing.mag2() <= 0.0) {
        return 0.0;
    }
    const G4ThreeVector inNorm = incoming.unit();
    const G4ThreeVector outNorm = outgoing.unit();
    const G4double cosTheta = std::clamp(inNorm.dot(outNorm), -1.0, 1.0);
    return std::acos(cosTheta);
}

G4ThreeVector ArtAnalysisManager::ComputeInterceptAtZ(const G4ThreeVector& p1, const G4ThreeVector& p2, G4double targetZ)
{
    const G4double dz = p2.z() - p1.z();
    if (std::abs(dz) < 1e-12) {
        return G4ThreeVector(p2.x(), p2.y(), targetZ);
    }

    const G4double t = (targetZ - p1.z()) / dz;
    return G4ThreeVector(
        p1.x() + t * (p2.x() - p1.x()),
        p1.y() + t * (p2.y() - p1.y()),
        targetZ
    );
}

G4int ArtAnalysisManager::ComputeCellIndex(const G4ThreeVector& objectIntersection) const
{
    const G4double x = objectIntersection.x();
    const G4double y = objectIntersection.y();

    if (x < -fGridHalfWidth || x > fGridHalfWidth || y < -fGridHalfHeight || y > fGridHalfHeight) {
        return -1;
    }

    const G4double normalizedX = (x + fGridHalfWidth) / (2.0 * fGridHalfWidth);
    const G4double normalizedY = (y + fGridHalfHeight) / (2.0 * fGridHalfHeight);

    G4int ix = static_cast<G4int>(normalizedX * fGridNX);
    G4int iy = static_cast<G4int>(normalizedY * fGridNY);

    ix = std::clamp(ix, 0, fGridNX - 1);
    iy = std::clamp(iy, 0, fGridNY - 1);

    return iy * fGridNX + ix;
}

G4double ArtAnalysisManager::EstimateMaterialBudgetFromScatteringHighland(G4double scatteringAngle, G4double kineticEnergy)
{
    if (scatteringAngle <= 0.0 || kineticEnergy <= 0.0) {
        return 0.0;
    }

    constexpr G4double electronMassMeV = 0.51099895;
    const G4double kineticMeV = kineticEnergy / MeV;
    const G4double totalMeV = kineticMeV + electronMassMeV;
    const G4double momentumMeV = std::sqrt(std::max(0.0, totalMeV * totalMeV - electronMassMeV * electronMassMeV));
    if (momentumMeV <= 0.0) {
        return 0.0;
    }

    const G4double beta = std::clamp(momentumMeV / totalMeV, 1e-6, 1.0);
    const G4double k = 13.6 / (beta * momentumMeV); // theta0 = k*sqrt(t)*(1+0.038ln(t))
    if (k <= 0.0) {
        return 0.0;
    }

    // Solve for t = x/X0 with fixed-point iterations.
    G4double t = std::pow(scatteringAngle / k, 2.0);
    t = std::clamp(t, 1e-12, 10.0);
    for (int i = 0; i < 8; ++i) {
        const G4double correction = std::max(0.2, 1.0 + 0.038 * std::log(std::max(1e-12, t)));
        const G4double nextT = std::pow(scatteringAngle / (k * correction), 2.0);
        t = std::clamp(nextT, 1e-12, 10.0);
    }

    return t;
}

G4double ArtAnalysisManager::ComputeTransmission(G4int incidentCount, G4int transmittedCount)
{
    if (incidentCount <= 0 || transmittedCount <= 0) {
        return 0.0;
    }
    if (transmittedCount >= incidentCount) {
        return 1.0;
    }
    return static_cast<G4double>(transmittedCount) / static_cast<G4double>(incidentCount);
}

G4double ArtAnalysisManager::ComputeProjectionAngleY(const G4ThreeVector& direction)
{
    if (direction.mag2() <= 0.0) {
        return 0.0;
    }
    const G4ThreeVector unitDir = direction.unit();
    return std::atan2(unitDir.y(), unitDir.z());
}

G4double ArtAnalysisManager::EstimateThicknessFromStoppingPower(G4double energyLoss, G4double stoppingPower)
{
    if (energyLoss <= 0.0 || stoppingPower <= 0.0) {
        return 0.0;
    }
    return energyLoss / stoppingPower;
}

G4double ArtAnalysisManager::EstimateRadiationLengthFromThicknessAndBudget(G4double thickness, G4double xOverX0)
{
    if (thickness <= 0.0 || xOverX0 <= 0.0) {
        return 0.0;
    }
    return thickness / xOverX0;
}

G4double ArtAnalysisManager::EstimateLinearAttenuationFromTransmission(G4double transmission, G4double thickness)
{
    if (thickness <= 0.0 || transmission <= 0.0 || transmission > 1.0) {
        return 0.0;
    }
    return -std::log(transmission) / thickness;
}

G4double ArtAnalysisManager::ComputeEnergyLossFromCalorimeters(
    G4double upstreamEnergy, G4bool hasUpstreamEnergy,
    G4double downstreamEnergy, G4bool hasDownstreamEnergy,
    G4double initialEnergyFallback, G4double finalEnergyFallback)
{
    if (hasUpstreamEnergy && hasDownstreamEnergy) {
        return std::max(0.0, upstreamEnergy - downstreamEnergy);
    }
    if (hasUpstreamEnergy) {
        return std::max(0.0, upstreamEnergy - finalEnergyFallback);
    }
    if (hasDownstreamEnergy) {
        return std::max(0.0, initialEnergyFallback - downstreamEnergy);
    }
    return std::max(0.0, initialEnergyFallback - finalEnergyFallback);
}

G4double ArtAnalysisManager::ComputeTrackerAngularResolution(G4double positionSigma, G4double leverArm)
{
    if (positionSigma <= 0.0 || leverArm <= 0.0) {
        return 0.0;
    }
    return std::sqrt(2.0) * positionSigma / leverArm;
}

G4double ArtAnalysisManager::ComputeCalorimeterEnergySigma(
    G4double energy, G4double stochasticTerm, G4double constantTerm, G4double noiseTerm)
{
    if (energy <= 0.0) {
        return 0.0;
    }

    const G4double stoch = std::max(0.0, stochasticTerm);
    const G4double constant = std::max(0.0, constantTerm);
    const G4double noise = std::max(0.0, noiseTerm);
    const G4double energyGeV = std::max(1e-9, energy / GeV);

    const G4double termStochastic = stoch / std::sqrt(energyGeV);
    const G4double termNoise = noise / energy;
    const G4double frac = std::sqrt(termStochastic * termStochastic + constant * constant + termNoise * termNoise);
    return frac * energy;
}

G4double ArtAnalysisManager::ClampProbability(G4double probability)
{
    return std::clamp(probability, 0.0, 1.0);
}

G4bool ArtAnalysisManager::ShouldAcceptByEfficiency(G4double randomUniform, G4double efficiency)
{
    const G4double eff = ClampProbability(efficiency);
    return randomUniform <= eff;
}

G4bool ArtAnalysisManager::ShouldInjectFakeHit(G4double randomUniform, G4double fakeProbability)
{
    const G4double prob = ClampProbability(fakeProbability);
    return randomUniform <= prob;
}

G4bool ArtAnalysisManager::InferMaterialAndThicknessFromObservables(
    G4double xOverX0,
    G4double energyLoss,
    G4double kineticEnergy,
    G4String& materialName,
    G4double& thickness,
    G4double& radiationLength,
    G4double& stoppingPower) const
{
    if (xOverX0 <= 0.0 || energyLoss <= 0.0 || kineticEnergy <= 0.0) {
        return false;
    }

    const std::array<G4String, 9> candidates = {{
        "LeadWhite",
        "ModernPaint",
        "Varnish",
        "Vermilion",
        "LapisLazuli",
        "Malachite",
        "LeadTinYellow",
        "Gesso",
        "Canvas",
    }};

    G4double bestScore = std::numeric_limits<G4double>::max();
    G4String bestMaterial = "";
    G4double bestThickness = 0.0;
    G4double bestX0 = 0.0;
    G4double bestDedx = 0.0;

    for (const auto& candidate : candidates) {
        G4Material* mat = G4Material::GetMaterial(candidate, false);
        if (mat == nullptr) {
            continue;
        }
        const G4double x0 = mat->GetRadlen();
        if (x0 <= 0.0) {
            continue;
        }
        const G4double dedx = CalculateStoppingPower(candidate, kineticEnergy);
        if (dedx <= 0.0) {
            continue;
        }

        const G4double x = xOverX0 * x0;
        const G4double predictedEnergyLoss = x * dedx;
        const G4double denom = std::max(1e-9 * MeV, energyLoss);
        const G4double relError = std::abs(predictedEnergyLoss - energyLoss) / denom;
        const G4double score = relError;
        if (score < bestScore) {
            bestScore = score;
            bestMaterial = candidate;
            bestThickness = x;
            bestX0 = x0;
            bestDedx = dedx;
        }
    }

    if (bestMaterial.empty()) {
        return false;
    }

    materialName = bestMaterial;
    thickness = bestThickness;
    radiationLength = bestX0;
    stoppingPower = bestDedx;
    return true;
}
