#include "ArtTrackingAction.hh"
#include "G4Electron.hh"
#include "G4Track.hh"

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
    if (track->GetParentID() != 0 || track->GetDefinition() != G4Electron::Definition()) {
        return;
    }

    fEventAction->SetPrimaryState(
        track->GetTrackID(),
        track->GetKineticEnergy(),
        track->GetPosition(),
        track->GetMomentumDirection());
}

void ArtTrackingAction::PostUserTrackingAction(const G4Track* track)
{
    if (track->GetParentID() != 0 || track->GetDefinition() != G4Electron::Definition()) {
        return;
    }

    fEventAction->SetFinalTrackEnergy(track->GetKineticEnergy());
}
