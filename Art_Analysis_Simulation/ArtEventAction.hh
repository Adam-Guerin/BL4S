#ifndef ArtEventAction_h
#define ArtEventAction_h 1

#include "G4UserEventAction.hh"
#include "ArtAnalysisManager.hh"
#include <map>

class ArtDetectorConstruction;

struct DetectorHit {
    G4bool valid = false;
    G4ThreeVector position = G4ThreeVector();
    G4ThreeVector direction = G4ThreeVector(0, 0, 1);
    G4double kineticEnergy = 0.0;
    G4double energyDeposit = 0.0;
};

class ArtEventAction : public G4UserEventAction
{
public:
    explicit ArtEventAction(ArtDetectorConstruction* detectorConstruction);
    virtual ~ArtEventAction();

    virtual void BeginOfEventAction(const G4Event* event) override;
    virtual void EndOfEventAction(const G4Event* event) override;

    // Legacy data collection
    void AddEnergyDeposit(const G4String& material, G4double energy);
    void AddScatteringAngle(const G4String& material, G4double angle);
    void SetCurrentTrack(const ElectronTrack& track);

    // BFS-ready event observables
    void SetPrimaryState(G4int trackID, G4double kineticEnergy, const G4ThreeVector& position, const G4ThreeVector& direction);
    void SetFinalTrackEnergy(G4double kineticEnergy);
    void RecordTrackerHit(const G4String& detectorName, const G4ThreeVector& position,
                          const G4ThreeVector& direction, G4double kineticEnergy);
    void RecordCalorimeterStep(const G4String& detectorName, G4double kineticEnergy, G4double energyDeposit);
    void RecordTriggerHit();
    void SetInstrumentResponseEnabled(G4bool enabled);
    void SetTrackerPositionResolution(G4double sigma);
    void SetCalorimeterResolution(G4double stochasticTerm, G4double constantTerm, G4double noiseTerm);
    void SetTrackerEfficiency(G4double efficiency);
    void SetTriggerEfficiency(G4double efficiency);
    void SetCalorimeterEfficiency(G4double efficiency);
    void SetFakeHitProbabilities(G4double trackerFakeProbability, G4double calorimeterFakeProbability, G4double triggerFakeProbability);
    void SetProjectionAngleY(G4double angle);

private:
    ArtAnalysisManager* fAnalysisManager;
    ArtDetectorConstruction* fDetectorConstruction;
    ElectronTrack fCurrentTrack;
    G4bool fTrackStarted;

    G4int fCurrentEventID;
    G4double fInitialEnergy;
    G4double fFinalEnergyFallback;
    G4bool fHasPrimaryState;
    G4bool fHasTriggerHit;
    G4bool fEnableInstrumentResponse;
    G4double fTrackerPositionSigma;
    G4double fCaloStochasticTerm;
    G4double fCaloConstantTerm;
    G4double fCaloNoiseTerm;
    G4double fTrackerEfficiency;
    G4double fTriggerEfficiency;
    G4double fCalorimeterEfficiency;
    G4double fTrackerFakeProbability;
    G4double fCalorimeterFakeProbability;
    G4double fTriggerFakeProbability;
    G4double fTransmissionMinEnergy;
    G4double fTransmissionMaxAngle;
    G4double fProjectionAngleY;
    std::map<G4String, DetectorHit> fTrackerHits;
    std::map<G4String, DetectorHit> fCalorimeterHits;
};

#endif
