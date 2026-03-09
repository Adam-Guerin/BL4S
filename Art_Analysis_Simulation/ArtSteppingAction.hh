#ifndef ArtSteppingAction_h
#define ArtSteppingAction_h 1

#include "ArtEventAction.hh"
#include "G4UserSteppingAction.hh"

class ArtSteppingAction : public G4UserSteppingAction
{
public:
    ArtSteppingAction(ArtEventAction* eventAction);
    virtual ~ArtSteppingAction();

    virtual void UserSteppingAction(const G4Step* step) override;

private:
    ArtEventAction* fEventAction;
};

#endif
