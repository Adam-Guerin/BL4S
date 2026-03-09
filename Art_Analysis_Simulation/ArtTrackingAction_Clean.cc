#include "ArtTrackingAction.hh"
#include "G4Track.hh"
#include "G4Electron.hh"
#include "G4SystemOfUnits.hh"

ArtTrackingAction::ArtTrackingAction(ArtEventAction* eventAction)
: G4UserTrackingAction(),
  fEventAction(eventAction)
{
}

ArtTrackingAction::~ArtTrackingAction()
{
}

void ArtTrackingAction::PreUserTrackingAction(const G4Track* track)
{
    // Only track primary electrons
    if (track->GetParentID() == 0 && track->GetDefinition() == G4Electron::Definition()) {
        ElectronTrack electronTrack;
        electronTrack.trackID = track->GetTrackID();
        electronTrack.initialEnergy = track->GetKineticEnergy();
        electronTrack.initialPosition = track->GetPosition();
        electronTrack.initialMomentum = track->GetMomentumDirection();
        electronTrack.finalEnergy = track->GetKineticEnergy();
        electronTrack.finalPosition = track->GetPosition();
        electronTrack.finalMomentum = track->GetMomentumDirection();
        electronTrack.pathLength = 0.0;
        electronTrack.scatteringAngle = 0.0;
        
        fEventAction->SetCurrentTrack(electronTrack);
    }
}

void ArtTrackingAction::PostUserTrackingAction(const G4Track* track)
{
    // Only track primary electrons
    if (track->GetParentID() == 0 && track->GetDefinition() == G4Electron::Definition()) {
        ElectronTrack electronTrack;
        electronTrack.trackID = track->GetTrackID();
        electronTrack.initialEnergy = track->GetCreatorProcess() ? 
                                  track->GetCreatorProcess()->GetParentID() : track->GetKineticEnergy();
        electronTrack.finalEnergy = track->GetKineticEnergy();
        electronTrack.pathLength = track->GetStepLength();
        
        // Calculate scattering angle
        G4ThreeVector initialDir = track->GetVertexMomentumDirection();
        G4ThreeVector finalDir = track->GetMomentumDirection();
        G4double cosTheta = initialDir.dot(finalDir);
        electronTrack.scatteringAngle = std::acos(std::clamp(cosTheta, -1.0, 1.0));
        
        fEventAction->SetCurrentTrack(electronTrack);
    }
}
