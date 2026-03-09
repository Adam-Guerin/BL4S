#ifndef ArtAnalysisManager_h
#define ArtAnalysisManager_h 1

#include "G4ThreeVector.hh"
#include "G4String.hh"
#include <vector>
#include <map>

struct ElectronTrack {
    G4int trackID;
    G4double initialEnergy;
    G4double finalEnergy;
    G4ThreeVector initialPosition;
    G4ThreeVector finalPosition;
    G4ThreeVector initialMomentum;
    G4ThreeVector finalMomentum;
    G4double pathLength;
    std::vector<G4String> materialsVisited;
    std::vector<G4double> energyDeposits;
    G4double scatteringAngle;
};

struct LayerAnalysis {
    G4String materialName;
    G4double inferredThickness;
    G4double meanEnergyDeposit;
    G4double meanScatteringAngle;
    G4int sampleCount;
    G4double density;
    G4double atomicNumber;
    G4bool isHistoricalPigment;
};

struct TomographyEvent {
    G4int eventID;
    G4bool hasTrigger;
    G4bool hasIncomingTrack;
    G4bool hasOutgoingTrack;
    G4bool hasCalorimeterIn;
    G4bool hasCalorimeterOut;
    G4bool hasCalorimeterEnergy;
    G4bool transmitted;
    G4ThreeVector incomingDirection;
    G4ThreeVector outgoingDirection;
    G4ThreeVector objectIntersection;
    G4double initialEnergy;
    G4double upstreamCalorimeterEnergy;
    G4double downstreamCalorimeterEnergy;
    G4double finalEnergy;
    G4double energyLoss;
    G4double scatteringAngle;
    G4double estimatedXOverX0;
    G4double estimatedThickness;
    G4double estimatedRadiationLength;
    G4double estimatedStoppingPower;
    G4double estimatedMu;
    G4double estimatedTransmission;
    G4String inferredMaterial;
    G4double projectionAngleY;
};

class ArtAnalysisManager
{
public:
    static ArtAnalysisManager* Instance();
    
    // Data collection
    void AddElectronTrack(const ElectronTrack& track);
    void RecordEnergyDeposit(const G4String& material, G4double energy);
    void RecordScattering(const G4String& material, G4double angle);
    void AddTomographyEvent(const TomographyEvent& eventRecord);
    void ConfigureGrid(G4int nX, G4int nY, G4double halfWidth, G4double halfHeight);

    // Analysis
    void AnalyzeLayers();
    void GenerateReport();
    void SaveResults(const G4String& filename);
    void AnalyzeThicknessByEnergyLoss(); // Nouvelle méthode d'analyse d'épaisseur
    void DetectHistoricalPigments(); // Détection des pigments historiques
    void CalculateScatteringSignatures(); // Signatures de diffusion caractéristiques

    // Geometry helpers used by tracking and tests
    static G4double ComputeScatteringAngle(const G4ThreeVector& incoming, const G4ThreeVector& outgoing);
    static G4ThreeVector ComputeInterceptAtZ(const G4ThreeVector& p1, const G4ThreeVector& p2, G4double targetZ);
    static G4double EstimateMaterialBudgetFromScatteringHighland(G4double scatteringAngle, G4double kineticEnergy);
    static G4double ComputeTransmission(G4int incidentCount, G4int transmittedCount);
    static G4double ComputeProjectionAngleY(const G4ThreeVector& direction);
    static G4double EstimateThicknessFromStoppingPower(G4double energyLoss, G4double stoppingPower);
    static G4double EstimateRadiationLengthFromThicknessAndBudget(G4double thickness, G4double xOverX0);
    static G4double EstimateLinearAttenuationFromTransmission(G4double transmission, G4double thickness);
    static G4double ComputeEnergyLossFromCalorimeters(
        G4double upstreamEnergy, G4bool hasUpstreamEnergy,
        G4double downstreamEnergy, G4bool hasDownstreamEnergy,
        G4double initialEnergyFallback, G4double finalEnergyFallback);
    static G4double ComputeTrackerAngularResolution(G4double positionSigma, G4double leverArm);
    static G4double ComputeCalorimeterEnergySigma(
        G4double energy, G4double stochasticTerm, G4double constantTerm, G4double noiseTerm);
    static G4double ClampProbability(G4double probability);
    static G4bool ShouldAcceptByEfficiency(G4double randomUniform, G4double efficiency);
    static G4bool ShouldInjectFakeHit(G4double randomUniform, G4double fakeProbability);
    
    // Accessors
    const std::vector<ElectronTrack>& GetTracks() const { return fElectronTracks; }
    const std::vector<LayerAnalysis>& GetLayerAnalysis() const { return fLayerAnalysis; }
    const std::vector<TomographyEvent>& GetTomographyEvents() const { return fTomographyEvents; }
    
    // Reset
    void Reset();
    
private:
    static ArtAnalysisManager* fInstance;
    
    ArtAnalysisManager();
    virtual ~ArtAnalysisManager();
    
    std::vector<ElectronTrack> fElectronTracks;
    std::vector<TomographyEvent> fTomographyEvents;
    std::map<G4String, std::vector<G4double>> fEnergyDepositsByMaterial;
    std::map<G4String, std::vector<G4double>> fScatteringAnglesByMaterial;
    std::vector<LayerAnalysis> fLayerAnalysis;
    G4int fGridNX;
    G4int fGridNY;
    G4double fGridHalfWidth;
    G4double fGridHalfHeight;
    std::vector<G4int> fCellCounts;
    std::vector<G4int> fCellIncidentCounts;
    std::vector<G4int> fCellTransmittedCounts;
    std::vector<G4double> fCellScatteringSum;
    std::vector<G4double> fCellEnergyLossSum;
    std::vector<G4double> fCellThicknessSum;
    
    // Analysis methods
    G4double CalculateMean(const std::vector<G4double>& values);
    G4double CalculateStdDev(const std::vector<G4double>& values);
    G4double CalculateMaterialZ(const G4String& material);
    G4double EstimateThickness(const G4String& material, G4double energyLoss, G4double kineticEnergy);
    G4double CalculateStoppingPower(const G4String& material, G4double energy);
    G4bool IsHistoricalPigment(const G4String& material);
    G4double CalculateDensity(const G4String& material);
    G4int ComputeCellIndex(const G4ThreeVector& objectIntersection) const;
    G4bool InferMaterialAndThicknessFromObservables(
        G4double xOverX0,
        G4double energyLoss,
        G4double kineticEnergy,
        G4String& materialName,
        G4double& thickness,
        G4double& radiationLength,
        G4double& stoppingPower) const;
};

#endif
