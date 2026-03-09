#include "ArtEventAction.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <algorithm>
#include <array>

ArtEventAction::ArtEventAction()
    : G4UserEventAction(),
      fAnalysisManager(ArtAnalysisManager::Instance()),
      fTrackStarted(false),
      fCurrentEventID(-1),
      fInitialEnergy(0.0),
      fFinalEnergyFallback(0.0),
      fHasPrimaryState(false),
      fHasTriggerHit(false),
      fEnableInstrumentResponse(true),
      fTrackerPositionSigma(50.0 * micrometer),
      fCaloStochasticTerm(0.10),
      fCaloConstantTerm(0.01),
      fCaloNoiseTerm(2.0 * MeV),
      fTrackerEfficiency(0.98),
      fTriggerEfficiency(0.995),
      fCalorimeterEfficiency(0.99),
      fTrackerFakeProbability(0.002),
      fCalorimeterFakeProbability(0.001),
      fTriggerFakeProbability(0.0005)
{
}

ArtEventAction::~ArtEventAction()
{
}

void ArtEventAction::BeginOfEventAction(const G4Event* event)
{
    fTrackStarted = false;
    fCurrentTrack = ElectronTrack();
    fCurrentEventID = event->GetEventID();
    fInitialEnergy = 0.0;
    fFinalEnergyFallback = 0.0;
    fHasPrimaryState = false;
    fHasTriggerHit = false;
    fTrackerHits.clear();
    fCalorimeterHits.clear();

    if (ArtAnalysisManager::ShouldInjectFakeHit(G4UniformRand(), fTriggerFakeProbability)) {
        fHasTriggerHit = true;
    }

    struct TrackerPlane {
        const char* name;
        G4double z;
    };
    const std::array<TrackerPlane, 4> trackerPlanes = {{
        {"TrackerUp1", -30.0 * cm},
        {"TrackerUp2", -20.0 * cm},
        {"TrackerDown1", 20.0 * cm},
        {"TrackerDown2", 30.0 * cm},
    }};
    for (const auto& plane : trackerPlanes) {
        if (!ArtAnalysisManager::ShouldInjectFakeHit(G4UniformRand(), fTrackerFakeProbability)) {
            continue;
        }
        DetectorHit& fakeHit = fTrackerHits[plane.name];
        fakeHit.valid = true;
        fakeHit.position = G4ThreeVector(
            G4RandFlat::shoot(-12.0 * cm, 12.0 * cm),
            G4RandFlat::shoot(-12.0 * cm, 12.0 * cm),
            plane.z);
        fakeHit.direction = G4ThreeVector(0.0, 0.0, 1.0);
        fakeHit.kineticEnergy = 2.0 * GeV;
    }

    struct CaloPlane {
        const char* name;
        G4double z;
    };
    const std::array<CaloPlane, 2> caloPlanes = {{
        {"CalorimeterIn", -10.0 * cm},
        {"CalorimeterOut", 40.0 * cm},
    }};
    for (const auto& plane : caloPlanes) {
        if (!ArtAnalysisManager::ShouldInjectFakeHit(G4UniformRand(), fCalorimeterFakeProbability)) {
            continue;
        }
        DetectorHit& fakeHit = fCalorimeterHits[plane.name];
        fakeHit.valid = true;
        fakeHit.position = G4ThreeVector(0.0, 0.0, plane.z);
        fakeHit.direction = G4ThreeVector(0.0, 0.0, 1.0);
        fakeHit.kineticEnergy = G4RandFlat::shoot(5.0 * MeV, 300.0 * MeV);
    }
}

void ArtEventAction::EndOfEventAction(const G4Event*)
{
    if (fTrackStarted) {
        fAnalysisManager->AddElectronTrack(fCurrentTrack);
    }

    TomographyEvent eventRecord{};
    eventRecord.eventID = fCurrentEventID;
    eventRecord.hasTrigger = fHasTriggerHit;
    eventRecord.hasIncomingTrack = false;
    eventRecord.hasOutgoingTrack = false;
    const auto calIn = fCalorimeterHits.find("CalorimeterIn");
    const auto calOut = fCalorimeterHits.find("CalorimeterOut");
    const G4bool hasCalIn = (calIn != fCalorimeterHits.end() && calIn->second.valid);
    const G4bool hasCalOut = (calOut != fCalorimeterHits.end() && calOut->second.valid);
    eventRecord.hasCalorimeterIn = hasCalIn;
    eventRecord.hasCalorimeterOut = hasCalOut;
    eventRecord.hasCalorimeterEnergy = hasCalOut;
    eventRecord.transmitted = fHasTriggerHit && hasCalOut;
    eventRecord.initialEnergy = fInitialEnergy;
    eventRecord.upstreamCalorimeterEnergy = hasCalIn ? calIn->second.kineticEnergy : 0.0;
    eventRecord.downstreamCalorimeterEnergy = hasCalOut ? calOut->second.kineticEnergy : 0.0;
    if (hasCalOut) {
        eventRecord.finalEnergy = calOut->second.kineticEnergy;
    } else {
        eventRecord.finalEnergy = fFinalEnergyFallback;
    }
    eventRecord.energyLoss = ArtAnalysisManager::ComputeEnergyLossFromCalorimeters(
        hasCalIn ? calIn->second.kineticEnergy : 0.0, hasCalIn,
        hasCalOut ? calOut->second.kineticEnergy : 0.0, hasCalOut,
        eventRecord.initialEnergy, eventRecord.finalEnergy);
    eventRecord.incomingDirection = G4ThreeVector(0, 0, 1);
    eventRecord.outgoingDirection = G4ThreeVector(0, 0, 1);
    eventRecord.objectIntersection = G4ThreeVector();
    eventRecord.scatteringAngle = 0.0;
    eventRecord.estimatedXOverX0 = 0.0;
    eventRecord.projectionAngleY = 0.0;

    const auto up1 = fTrackerHits.find("TrackerUp1");
    const auto up2 = fTrackerHits.find("TrackerUp2");
    if (up1 != fTrackerHits.end() && up2 != fTrackerHits.end() && up1->second.valid && up2->second.valid) {
        eventRecord.hasIncomingTrack = true;
        eventRecord.incomingDirection = (up2->second.position - up1->second.position).unit();
        eventRecord.objectIntersection = ArtAnalysisManager::ComputeInterceptAtZ(
            up1->second.position, up2->second.position, 0.0);
    }

    const auto down1 = fTrackerHits.find("TrackerDown1");
    const auto down2 = fTrackerHits.find("TrackerDown2");
    if (down1 != fTrackerHits.end() && down2 != fTrackerHits.end() && down1->second.valid && down2->second.valid) {
        eventRecord.hasOutgoingTrack = true;
        eventRecord.outgoingDirection = (down2->second.position - down1->second.position).unit();
    }

    if (eventRecord.hasIncomingTrack && eventRecord.hasOutgoingTrack) {
        eventRecord.scatteringAngle = ArtAnalysisManager::ComputeScatteringAngle(
            eventRecord.incomingDirection,
            eventRecord.outgoingDirection);
        eventRecord.estimatedXOverX0 = ArtAnalysisManager::EstimateMaterialBudgetFromScatteringHighland(
            eventRecord.scatteringAngle,
            eventRecord.initialEnergy);
    }
    if (eventRecord.hasIncomingTrack) {
        eventRecord.projectionAngleY = ArtAnalysisManager::ComputeProjectionAngleY(eventRecord.incomingDirection);
    }

    fAnalysisManager->AddTomographyEvent(eventRecord);
}

void ArtEventAction::AddEnergyDeposit(const G4String& material, G4double energy)
{
    fAnalysisManager->RecordEnergyDeposit(material, energy);
    if (fTrackStarted) {
        fCurrentTrack.energyDeposits.push_back(energy);
        fCurrentTrack.materialsVisited.push_back(material);
    }
}

void ArtEventAction::AddScatteringAngle(const G4String& material, G4double angle)
{
    fAnalysisManager->RecordScattering(material, angle);
}

void ArtEventAction::SetCurrentTrack(const ElectronTrack& track)
{
    fCurrentTrack = track;
    fTrackStarted = true;
}

void ArtEventAction::SetPrimaryState(G4int trackID, G4double kineticEnergy, const G4ThreeVector& position, const G4ThreeVector& direction)
{
    fHasPrimaryState = true;
    fInitialEnergy = kineticEnergy;

    ElectronTrack electronTrack;
    electronTrack.trackID = trackID;
    electronTrack.initialEnergy = kineticEnergy;
    electronTrack.finalEnergy = kineticEnergy;
    electronTrack.initialPosition = position;
    electronTrack.finalPosition = position;
    electronTrack.initialMomentum = direction;
    electronTrack.finalMomentum = direction;
    electronTrack.pathLength = 0.0;
    electronTrack.scatteringAngle = 0.0;
    SetCurrentTrack(electronTrack);
}

void ArtEventAction::SetFinalTrackEnergy(G4double kineticEnergy)
{
    fFinalEnergyFallback = kineticEnergy;
    if (fTrackStarted) {
        fCurrentTrack.finalEnergy = kineticEnergy;
    }
}

void ArtEventAction::RecordTrackerHit(const G4String& detectorName, const G4ThreeVector& position,
                                      const G4ThreeVector& direction, G4double kineticEnergy)
{
    DetectorHit& hit = fTrackerHits[detectorName];
    if (hit.valid) {
        return;
    }
    if (!ArtAnalysisManager::ShouldAcceptByEfficiency(G4UniformRand(), fTrackerEfficiency)) {
        return;
    }

    G4ThreeVector measuredPosition = position;
    if (fEnableInstrumentResponse && fTrackerPositionSigma > 0.0) {
        measuredPosition.setX(G4RandGauss::shoot(position.x(), fTrackerPositionSigma));
        measuredPosition.setY(G4RandGauss::shoot(position.y(), fTrackerPositionSigma));
    }

    hit.valid = true;
    hit.position = measuredPosition;
    hit.direction = direction;
    hit.kineticEnergy = kineticEnergy;
}

void ArtEventAction::RecordCalorimeterStep(const G4String& detectorName, G4double kineticEnergy, G4double energyDeposit)
{
    DetectorHit& hit = fCalorimeterHits[detectorName];
    if (!hit.valid) {
        if (!ArtAnalysisManager::ShouldAcceptByEfficiency(G4UniformRand(), fCalorimeterEfficiency)) {
            return;
        }
        hit.valid = true;
        G4double measuredEnergy = kineticEnergy;
        if (fEnableInstrumentResponse) {
            const G4double sigma = ArtAnalysisManager::ComputeCalorimeterEnergySigma(
                kineticEnergy, fCaloStochasticTerm, fCaloConstantTerm, fCaloNoiseTerm);
            measuredEnergy = std::max(0.0, G4RandGauss::shoot(kineticEnergy, sigma));
        }
        hit.kineticEnergy = measuredEnergy;
    }
    if (energyDeposit > 0.0) {
        hit.energyDeposit += energyDeposit;
    }
}

void ArtEventAction::RecordTriggerHit()
{
    if (!fHasTriggerHit && ArtAnalysisManager::ShouldAcceptByEfficiency(G4UniformRand(), fTriggerEfficiency)) {
        fHasTriggerHit = true;
    }
}

void ArtEventAction::SetInstrumentResponseEnabled(G4bool enabled)
{
    fEnableInstrumentResponse = enabled;
}

void ArtEventAction::SetTrackerPositionResolution(G4double sigma)
{
    fTrackerPositionSigma = std::max(0.0, sigma);
}

void ArtEventAction::SetCalorimeterResolution(G4double stochasticTerm, G4double constantTerm, G4double noiseTerm)
{
    fCaloStochasticTerm = std::max(0.0, stochasticTerm);
    fCaloConstantTerm = std::max(0.0, constantTerm);
    fCaloNoiseTerm = std::max(0.0, noiseTerm);
}

void ArtEventAction::SetTrackerEfficiency(G4double efficiency)
{
    fTrackerEfficiency = ArtAnalysisManager::ClampProbability(efficiency);
}

void ArtEventAction::SetTriggerEfficiency(G4double efficiency)
{
    fTriggerEfficiency = ArtAnalysisManager::ClampProbability(efficiency);
}

void ArtEventAction::SetCalorimeterEfficiency(G4double efficiency)
{
    fCalorimeterEfficiency = ArtAnalysisManager::ClampProbability(efficiency);
}

void ArtEventAction::SetFakeHitProbabilities(G4double trackerFakeProbability, G4double calorimeterFakeProbability, G4double triggerFakeProbability)
{
    fTrackerFakeProbability = ArtAnalysisManager::ClampProbability(trackerFakeProbability);
    fCalorimeterFakeProbability = ArtAnalysisManager::ClampProbability(calorimeterFakeProbability);
    fTriggerFakeProbability = ArtAnalysisManager::ClampProbability(triggerFakeProbability);
}
