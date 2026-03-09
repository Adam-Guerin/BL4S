#include "ArtDetectorConstruction.hh"
#include "ArtPhysicsList.hh"
#include "ArtPrimaryGeneratorAction.hh"
#include "ArtEventAction.hh"
#include "ArtTrackingAction.hh"
#include "ArtSteppingAction.hh"
#include "ArtAnalysisManager.hh"
#include "ArtUICommands.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4MTRunManager.hh"

#include "G4SystemOfUnits.hh"

int main(int argc, char** argv)
{
    // Detect interactive mode
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    
    // Construct the run manager
    G4MTRunManager* runManager = new G4MTRunManager();
    
    // Set mandatory initialization classes
    runManager->SetUserInitialization(new ArtDetectorConstruction());
    runManager->SetUserInitialization(new ArtPhysicsList());
    
    // Primary generator action
    runManager->SetUserAction(new ArtPrimaryGeneratorAction());
    
    // User actions
    ArtEventAction* eventAction = new ArtEventAction();
    runManager->SetUserAction(eventAction);
    runManager->SetUserAction(new ArtTrackingAction(eventAction));
    runManager->SetUserAction(new ArtSteppingAction(eventAction));
    
    // UI commands for analysis (create but don't register as UserAction)
    ArtUICommands* uiCommands = new ArtUICommands(ArtAnalysisManager::Instance());
    // Note: UI commands are automatically registered by constructor
    
    // Initialize visualization
    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();
    
    // Get pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    
    // Process macro or start UI session
    if (argc > 1) {
        // Execute macro file
        G4String command = "/control/execute ";
        command += argv[1];
        UImanager->ApplyCommand(command);
    } else {
        // Start interactive session
        UImanager->ApplyCommand("/run/initialize");
        ui->SessionStart();
    }
    
    // Cleanup
    delete visManager;
    delete runManager;
    delete ui;
    
    return 0;
}
