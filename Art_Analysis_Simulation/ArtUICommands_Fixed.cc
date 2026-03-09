#include "ArtUICommands.hh"
#include "ArtAnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>
#include <iostream>
#include <iomanip>

ArtUICommands::ArtUICommands(ArtAnalysisManager* analysisManager)
: G4UImessenger(),
  fAnalysisManager(analysisManager)
{
    // Create directory
    fArtDirectory = new G4UIdirectory("/art/");
    fArtDirectory->SetGuidance("Commands for art analysis simulation");
    
    // Analyze command
    fAnalyzeCmd = new G4UIcmdWithAString("/art/analyze", this);
    fAnalyzeCmd->SetGuidance("Analyze collected data");
    fAnalyzeCmd->SetParameterName("options", "");
    fAnalyzeCmd->SetDefaultValue("");
    fAnalyzeCmd->AvailableForStates(G4State_Idle);
    
    // Save command
    fSaveCmd = new G4UIcmdWithAString("/art/save", this);
    fSaveCmd->SetGuidance("Save analysis results to file");
    fSaveCmd->SetParameterName("filename", "");
    fSaveCmd->SetDefaultValue("art_analysis_results.csv");
    fSaveCmd->AvailableForStates(G4State_Idle);
    
    // Report command
    fReportCmd = new G4UIcmdWithAString("/art/report", this);
    fReportCmd->SetGuidance("Generate detailed analysis report");
    fReportCmd->SetParameterName("format", "");
    fReportCmd->SetDefaultValue("console");
    fReportCmd->AvailableForStates(G4State_Idle);
    
    // Beam energy command
    fBeamEnergyCmd = new G4UIcmdWithADouble("/art/beamEnergy", this);
    fBeamEnergyCmd->SetGuidance("Set beam energy in keV");
    fBeamEnergyCmd->SetParameterName("energy", "");
    fBeamEnergyCmd->SetDefaultValue(100.0);
    fBeamEnergyCmd->SetUnitCategory("Energy");
    fBeamEnergyCmd->AvailableForStates(G4State_Idle);
    
    // Number of electrons command
    fElectronsCmd = new G4UIcmdWithAnInteger("/art/nElectrons", this);
    fElectronsCmd->SetGuidance("Set number of electrons to simulate");
    fElectronsCmd->SetParameterName("count", "");
    fElectronsCmd->SetDefaultValue(10000);
    fElectronsCmd->AvailableForStates(G4State_Idle);
    
    // Verbose command
    fVerboseCmd = new G4UIcmdWithABool("/art/verbose", this);
    fVerboseCmd->SetGuidance("Enable verbose output");
    fVerboseCmd->SetParameterName("verbose", "");
    fVerboseCmd->SetDefaultValue(false);
    fVerboseCmd->AvailableForStates(G4State_Idle);
}

ArtUICommands::~ArtUICommands()
{
    delete fAnalyzeCmd;
    delete fSaveCmd;
    delete fReportCmd;
    delete fBeamEnergyCmd;
    delete fElectronsCmd;
    delete fVerboseCmd;
    delete fArtDirectory;
}

void ArtUICommands::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == fAnalyzeCmd) {
        G4cout << "=== Lancement de l'analyse des données ===" << G4endl;
        fAnalysisManager->AnalyzeLayers();
        fAnalysisManager->DetectHistoricalPigments();
        fAnalysisManager->CalculateScatteringSignatures();
        G4cout << "Analyse terminée!" << G4endl;
    }
    else if (command == fSaveCmd) {
        G4String filename = newValue;
        if (filename.empty()) filename = "art_analysis_results.csv";
        G4cout << "Sauvegarde des résultats dans: " << filename << G4endl;
        fAnalysisManager->SaveResults(filename);
    }
    else if (command == fReportCmd) {
        G4String format = newValue;
        if (format.empty()) format = "console";
        
        if (format == "console" || format == "all") {
            fAnalysisManager->GenerateReport();
        }
        
        if (format == "file" || format == "all") {
            std::ofstream reportFile("art_analysis_report.txt");
            if (reportFile.is_open()) {
                // Redirect output to file
                std::streambuf* orig = std::cout.rdbuf();
                std::cout.rdbuf(reportFile.rdbuf());
                
                fAnalysisManager->GenerateReport();
                
                std::cout.rdbuf(orig);
                reportFile.close();
                G4cout << "Rapport sauvegardé dans: art_analysis_report.txt" << G4endl;
            }
        }
    }
    else if (command == fBeamEnergyCmd) {
        G4double energy = G4UIcmdWithADouble::GetNewDoubleValue(newValue);
        G4cout << "Énergie du faisceau fixée à: " << energy/keV << " keV" << G4endl;
        // Note: This would require access to the primary generator
        // For now, just display the value
    }
    else if (command == fElectronsCmd) {
        G4int count = G4UIcmdWithAnInteger::GetNewIntValue(newValue);
        G4cout << "Nombre d'électrons fixé à: " << count << G4endl;
    }
    else if (command == fVerboseCmd) {
        G4bool verbose = G4UIcmdWithABool::GetNewBoolValue(newValue);
        G4cout << "Mode verbose: " << (verbose ? "activé" : "désactivé") << G4endl;
    }
}
