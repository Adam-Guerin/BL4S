#ifndef ArtTrackingAction_h
#define ArtTrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "ArtEventAction.hh"

class ArtTrackingAction : public G4UserTrackingAction
{
public:
    ArtTrackingAction(ArtEventAction* eventAction);
    virtual ~ArtTrackingAction();
    
    virtual void PreUserTrackingAction(const G4Track* track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;
    
private:
    ArtEventAction* fEventAction;
};

#endif
