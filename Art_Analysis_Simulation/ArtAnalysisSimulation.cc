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

#include "G4SystemOfUnits.hh"

#include <cstdlib>
#include <filesystem>
#include <string>

int main(int argc, char** argv)
{
    std::string ensdfPath;
#ifdef _WIN32
    char* envBuffer = nullptr;
    size_t envSize = 0;
    if (_dupenv_s(&envBuffer, &envSize, "G4ENSDFSTATEDATA") == 0 && envBuffer != nullptr) {
        ensdfPath = envBuffer;
        free(envBuffer);
    }
#else
    const char* envValue = std::getenv("G4ENSDFSTATEDATA");
    if (envValue != nullptr) {
        ensdfPath = envValue;
    }
#endif

    if (ensdfPath.empty() || !std::filesystem::exists(std::filesystem::path(ensdfPath) / "ENSDFSTATE.dat")) {
        G4cerr << "Missing Geant4 dataset: G4ENSDFSTATEDATA/ENSDFSTATE.dat" << G4endl;
        G4cerr << "Please configure Geant4 data before running the simulation." << G4endl;
        return 2;
    }

    G4String macroFile;
    G4bool keepInteractiveAfterMacro = false;
    if (argc > 1) {
        if (std::string(argv[1]) == "--vis" && argc > 2) {
            macroFile = argv[2];
            keepInteractiveAfterMacro = true;
        } else {
            macroFile = argv[1];
        }
    }

    const G4bool isBatchMode = (!macroFile.empty() && !keepInteractiveAfterMacro);
    G4UIExecutive* ui = nullptr;
    if (!isBatchMode || keepInteractiveAfterMacro) {
        ui = new G4UIExecutive(argc, argv);
    }
    
    // Use single-thread run manager to keep action setup simple and robust.
    G4RunManager* runManager = new G4RunManager();
    
    // Set mandatory initialization classes
    ArtDetectorConstruction* detectorConstruction = new ArtDetectorConstruction();
    runManager->SetUserInitialization(detectorConstruction);
    runManager->SetUserInitialization(new ArtPhysicsList());
    
    // Primary generator action
    ArtPrimaryGeneratorAction* primaryGenerator = new ArtPrimaryGeneratorAction();
    runManager->SetUserAction(primaryGenerator);
    
    // User actions
    ArtEventAction* eventAction = new ArtEventAction(detectorConstruction);
    runManager->SetUserAction(eventAction);
    runManager->SetUserAction(new ArtTrackingAction(eventAction));
    runManager->SetUserAction(new ArtSteppingAction(eventAction));
    
    // UI commands for analysis (create but don't register as UserAction)
    ArtUICommands* uiCommands = new ArtUICommands(ArtAnalysisManager::Instance(), primaryGenerator, eventAction, detectorConstruction);
    (void)uiCommands;
    
    // Initialize visualization in all modes so macros with /vis commands work.
    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();
    
    // Get pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    
    // Process macro or start UI session
    if (!macroFile.empty()) {
        // Execute macro file
        G4String command = "/control/execute ";
        command += macroFile;
        G4int status = UImanager->ApplyCommand(command);
        if (status != 0) {
            G4cerr << "Batch macro execution failed with status: " << status << G4endl;
            if (!keepInteractiveAfterMacro) {
                if (visManager) {
                    delete visManager;
                }
                delete uiCommands;
                delete runManager;
                if (ui) {
                    delete ui;
                }
                return 1;
            }
        }

        if (keepInteractiveAfterMacro && ui) {
            ui->SessionStart();
        }
    } else {
        // Start interactive session
        UImanager->ApplyCommand("/run/initialize");
        ui->SessionStart();
    }
    
    // Cleanup
    if (visManager) {
        delete visManager;
    }
    delete uiCommands;
    delete runManager;
    if (ui) {
        delete ui;
    }
    
    return 0;
}
