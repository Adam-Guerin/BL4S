#include "ArtSteppingAction.hh"
#include "ArtAnalysisManager.hh"

#include "G4Electron.hh"
#include "G4Material.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"

ArtSteppingAction::ArtSteppingAction(ArtEventAction* eventAction)
    : G4UserSteppingAction(),
      fEventAction(eventAction)
{
}

ArtSteppingAction::~ArtSteppingAction()
{
}

void ArtSteppingAction::UserSteppingAction(const G4Step* step)
{
    G4Track* track = step->GetTrack();

    if (track->GetParentID() != 0 || track->GetDefinition() != G4Electron::Definition()) {
        return;
    }

    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4StepPoint* postStepPoint = step->GetPostStepPoint();

    if (preStepPoint->GetMaterial() != nullptr) {
        const G4double energyDeposit = step->GetTotalEnergyDeposit();
        if (energyDeposit > 0.0) {
            fEventAction->AddEnergyDeposit(preStepPoint->GetMaterial()->GetName(), energyDeposit);
        }
    }

    if (preStepPoint->GetPhysicalVolume() == nullptr) {
        return;
    }

    const G4String volumeName = preStepPoint->GetPhysicalVolume()->GetName();
    const G4ThreeVector position = preStepPoint->GetPosition();
    const G4ThreeVector direction = preStepPoint->GetMomentumDirection();
    const G4double kineticEnergy = preStepPoint->GetKineticEnergy();

    if (volumeName == "TrackerUp1" || volumeName == "TrackerUp2" ||
        volumeName == "TrackerDown1" || volumeName == "TrackerDown2") {
        fEventAction->RecordTrackerHit(volumeName, position, direction, kineticEnergy);
    }

    if (volumeName == "TriggerScintillator") {
        fEventAction->RecordTriggerHit();
    }

    if (volumeName == "CalorimeterIn" || volumeName == "CalorimeterOut") {
        fEventAction->RecordCalorimeterStep(volumeName, kineticEnergy, step->GetTotalEnergyDeposit());
    }

    if (postStepPoint->GetStepStatus() == fGeomBoundary && postStepPoint->GetPhysicalVolume() != nullptr) {
        const G4ThreeVector inDir = preStepPoint->GetMomentumDirection();
        const G4ThreeVector outDir = postStepPoint->GetMomentumDirection();
        const G4double angle = ArtAnalysisManager::ComputeScatteringAngle(inDir, outDir);
        if (angle > 0.0 && preStepPoint->GetMaterial() != nullptr) {
            fEventAction->AddScatteringAngle(preStepPoint->GetMaterial()->GetName(), angle);
        }
    }
}
