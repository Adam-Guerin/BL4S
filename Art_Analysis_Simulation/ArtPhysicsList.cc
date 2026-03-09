#include "ArtPhysicsList.hh"

#include "G4EmStandardPhysics.hh"
#include "G4StepLimiterPhysics.hh"

ArtPhysicsList::ArtPhysicsList()
: G4VModularPhysicsList()
{
    // Electron-beam artwork analysis relies on electromagnetic interactions.
    // Keeping the physics list EM-only avoids unnecessary hadronic datasets.
    RegisterPhysics(new G4EmStandardPhysics());

    // Step limiter to prevent infinite loops
    RegisterPhysics(new G4StepLimiterPhysics());
}

ArtPhysicsList::~ArtPhysicsList()
{
}
