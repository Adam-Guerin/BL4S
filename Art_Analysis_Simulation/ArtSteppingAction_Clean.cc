#include "ArtSteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
#include "G4Electron.hh"
#include "G4SystemOfUnits.hh"
#include <iomanip>

ArtSteppingAction::ArtSteppingAction(ArtEventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fPreviousMaterial(""),
  fPreviousDirection(0, 0, 1)
{
}

ArtSteppingAction::~ArtSteppingAction()
{
}

void ArtSteppingAction::UserSteppingAction(const G4Step* step)
{
    G4Track* track = step->GetTrack();
    
    // Only process primary electrons
    if (track->GetParentID() != 0 || track->GetDefinition() != G4Electron::Definition()) {
        return;
    }
    
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4StepPoint* postStepPoint = step->GetPostStepPoint();
    
    // Get material information
    G4Material* material = preStepPoint->GetMaterial();
    G4String materialName = material->GetName();
    
    // Record energy deposit
    G4double energyDeposit = step->GetTotalEnergyDeposit();
    if (energyDeposit > 0) {
        fEventAction->AddEnergyDeposit(materialName, energyDeposit);
    }
    
    // Calculate scattering when entering new material
    if (postStepPoint->GetStepStatus() == fGeomBoundary && 
        postStepPoint->GetPhysicalVolume() != nullptr) {
        
        G4ThreeVector currentDirection = postStepPoint->GetMomentumDirection();
        
        if (!fPreviousMaterial.empty()) {
            G4double scatteringAngle = CalculateScatteringAngle(fPreviousDirection, currentDirection);
            if (scatteringAngle > 0) {
                fEventAction->AddScatteringAngle(fPreviousMaterial, scatteringAngle);
            }
        }
        
        fPreviousMaterial = materialName;
        fPreviousDirection = currentDirection;
    }
    
    // Special detection for trackers and calorimeters
    G4VPhysicalVolume* volume = preStepPoint->GetPhysicalVolume();
    if (volume) {
        G4String volumeName = volume->GetName();
        
        if (volumeName.find("Tracker") != G4String::npos) {
            // Record position at tracker locations
            G4ThreeVector position = preStepPoint->GetPosition();
            G4ThreeVector direction = preStepPoint->GetMomentumDirection();
            G4double kineticEnergy = preStepPoint->GetKineticEnergy();
            
            if (volumeName.find("Tracker1") != G4String::npos) {
                G4cout << std::fixed << std::setprecision(2);
                G4cout << "Tracker1 - Position: (" << position.x()/mm << ", " 
                       << position.y()/mm << ", " << position.z()/mm << ") mm, "
                       << "Énergie: " << kineticEnergy/keV << " keV" << G4endl;
            } else if (volumeName.find("Tracker2") != G4String::npos) {
                G4cout << std::fixed << std::setprecision(2);
                G4cout << "Tracker2 - Position: (" << position.x()/mm << ", " 
                       << position.y()/mm << ", " << position.z()/mm << ") mm, "
                       << "Énergie: " << kineticEnergy/keV << " keV" << G4endl;
            }
        }
        else if (volumeName.find("Calorimeter") != G4String::npos) {
            // Record energy at calorimeter positions
            G4double kineticEnergy = preStepPoint->GetKineticEnergy();
            
            if (volumeName.find("Calorimeter1") != G4String::npos) {
                // Entry calorimeter - record initial energy
                G4cout << std::fixed << std::setprecision(2);
                G4cout << "Calorimeter1 - Énergie entrée: " << kineticEnergy/keV << " keV" << G4endl;
                fEventAction->SetEntryEnergy(kineticEnergy);
            } else if (volumeName.find("Calorimeter2") != G4String::npos) {
                // Exit calorimeter - record final energy
                G4cout << std::fixed << std::setprecision(2);
                G4cout << "Calorimeter2 - Énergie sortie: " << kineticEnergy/keV << " keV" << G4endl;
                fEventAction->SetExitEnergy(kineticEnergy);
            }
        }
    }
}

G4double ArtSteppingAction::CalculateScatteringAngle(const G4ThreeVector& dir1, const G4ThreeVector& dir2)
{
    G4double cosTheta = dir1.dot(dir2);
    cosTheta = std::clamp(cosTheta, -1.0, 1.0);
    return std::acos(cosTheta);
}
