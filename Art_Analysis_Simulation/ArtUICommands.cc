#include "ArtUICommands.hh"

#include "ArtAnalysisManager.hh"
#include "ArtEventAction.hh"
#include "ArtPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

ArtUICommands::ArtUICommands(ArtAnalysisManager* analysisManager, ArtPrimaryGeneratorAction* primaryGenerator, ArtEventAction* eventAction)
    : G4UImessenger(),
      fAnalysisManager(analysisManager),
      fPrimaryGenerator(primaryGenerator),
      fEventAction(eventAction),
      fConfiguredCaloStochasticTerm(0.10),
      fConfiguredCaloConstantTerm(0.01),
      fConfiguredCaloNoiseTerm(2.0 * MeV),
      fConfiguredTrackerFakeProbability(0.002),
      fConfiguredCaloFakeProbability(0.001),
      fConfiguredTriggerFakeProbability(0.0005)
{
    fArtDirectory = new G4UIdirectory("/art/");
    fArtDirectory->SetGuidance("Commands for BFS-ready electron tomography");

    fAnalyzeCmd = new G4UIcmdWithAString("/art/analyze", this);
    fAnalyzeCmd->SetGuidance("Analyze collected data");
    fAnalyzeCmd->SetParameterName("options", true);
    fAnalyzeCmd->SetDefaultValue("");
    fAnalyzeCmd->AvailableForStates(G4State_Idle);

    fSaveCmd = new G4UIcmdWithAString("/art/save", this);
    fSaveCmd->SetGuidance("Save analysis results to file");
    fSaveCmd->SetParameterName("filename", true);
    fSaveCmd->SetDefaultValue("bfs_results.csv");
    fSaveCmd->AvailableForStates(G4State_Idle);

    fReportCmd = new G4UIcmdWithAString("/art/report", this);
    fReportCmd->SetGuidance("Generate report (console|file|all)");
    fReportCmd->SetParameterName("format", true);
    fReportCmd->SetDefaultValue("console");
    fReportCmd->AvailableForStates(G4State_Idle);

    fBeamEnergyCmd = new G4UIcmdWithADoubleAndUnit("/art/beamEnergy", this);
    fBeamEnergyCmd->SetGuidance("Set electron beam kinetic energy");
    fBeamEnergyCmd->SetParameterName("energy", false);
    fBeamEnergyCmd->SetDefaultUnit("GeV");
    fBeamEnergyCmd->SetUnitCandidates("MeV GeV");
    fBeamEnergyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fBeamSigmaCmd = new G4UIcmdWithADoubleAndUnit("/art/beamSigma", this);
    fBeamSigmaCmd->SetGuidance("Set transverse beam sigma");
    fBeamSigmaCmd->SetParameterName("sigma", false);
    fBeamSigmaCmd->SetDefaultUnit("mm");
    fBeamSigmaCmd->SetUnitCandidates("um mm cm");
    fBeamSigmaCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fBeamTiltYCmd = new G4UIcmdWithADoubleAndUnit("/art/beamTiltY", this);
    fBeamTiltYCmd->SetGuidance("Set beam tilt around X axis using angle in Y-Z plane");
    fBeamTiltYCmd->SetParameterName("angle", false);
    fBeamTiltYCmd->SetDefaultUnit("mrad");
    fBeamTiltYCmd->SetUnitCandidates("mrad rad deg");
    fBeamTiltYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fGridCmd = new G4UIcmdWithAnInteger("/art/grid", this);
    fGridCmd->SetGuidance("Set square tomography grid size N (NxN)");
    fGridCmd->SetParameterName("n", false);
    fGridCmd->SetDefaultValue(20);
    fGridCmd->AvailableForStates(G4State_Idle);

    fVerboseCmd = new G4UIcmdWithABool("/art/verbose", this);
    fVerboseCmd->SetGuidance("Enable verbose output");
    fVerboseCmd->SetParameterName("verbose", true);
    fVerboseCmd->SetDefaultValue(false);
    fVerboseCmd->AvailableForStates(G4State_Idle);

    fInstrumentResponseCmd = new G4UIcmdWithABool("/art/instrumentResponse", this);
    fInstrumentResponseCmd->SetGuidance("Enable or disable detector response smearing");
    fInstrumentResponseCmd->SetParameterName("enabled", true);
    fInstrumentResponseCmd->SetDefaultValue(true);
    fInstrumentResponseCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fTrackerResolutionCmd = new G4UIcmdWithADoubleAndUnit("/art/trackerResolution", this);
    fTrackerResolutionCmd->SetGuidance("Set tracker hit position resolution (Gaussian sigma)");
    fTrackerResolutionCmd->SetParameterName("sigma", false);
    fTrackerResolutionCmd->SetDefaultUnit("um");
    fTrackerResolutionCmd->SetUnitCandidates("um mm");
    fTrackerResolutionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fCaloNoiseCmd = new G4UIcmdWithADoubleAndUnit("/art/caloNoise", this);
    fCaloNoiseCmd->SetGuidance("Set calorimeter noise term (absolute sigma component)");
    fCaloNoiseCmd->SetParameterName("noise", false);
    fCaloNoiseCmd->SetDefaultUnit("MeV");
    fCaloNoiseCmd->SetUnitCandidates("keV MeV GeV");
    fCaloNoiseCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fCaloResolutionCmd = new G4UIcmdWithAString("/art/caloResolution", this);
    fCaloResolutionCmd->SetGuidance("Set calorimeter fractional terms: '<stochastic> <constant>' e.g. '0.10 0.01'");
    fCaloResolutionCmd->SetParameterName("terms", false);
    fCaloResolutionCmd->SetDefaultValue("0.10 0.01");
    fCaloResolutionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fTrackerEfficiencyCmd = new G4UIcmdWithADouble("/art/trackerEfficiency", this);
    fTrackerEfficiencyCmd->SetGuidance("Set tracker hit efficiency in [0,1]");
    fTrackerEfficiencyCmd->SetParameterName("eff", false);
    fTrackerEfficiencyCmd->SetDefaultValue(0.98);
    fTrackerEfficiencyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fTriggerEfficiencyCmd = new G4UIcmdWithADouble("/art/triggerEfficiency", this);
    fTriggerEfficiencyCmd->SetGuidance("Set trigger efficiency in [0,1]");
    fTriggerEfficiencyCmd->SetParameterName("eff", false);
    fTriggerEfficiencyCmd->SetDefaultValue(0.995);
    fTriggerEfficiencyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fCaloEfficiencyCmd = new G4UIcmdWithADouble("/art/caloEfficiency", this);
    fCaloEfficiencyCmd->SetGuidance("Set calorimeter hit efficiency in [0,1]");
    fCaloEfficiencyCmd->SetParameterName("eff", false);
    fCaloEfficiencyCmd->SetDefaultValue(0.99);
    fCaloEfficiencyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fTrackerFakeProbabilityCmd = new G4UIcmdWithADouble("/art/trackerFakeProb", this);
    fTrackerFakeProbabilityCmd->SetGuidance("Set per-event fake hit probability for each tracker plane in [0,1]");
    fTrackerFakeProbabilityCmd->SetParameterName("prob", false);
    fTrackerFakeProbabilityCmd->SetDefaultValue(fConfiguredTrackerFakeProbability);
    fTrackerFakeProbabilityCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fCaloFakeProbabilityCmd = new G4UIcmdWithADouble("/art/caloFakeProb", this);
    fCaloFakeProbabilityCmd->SetGuidance("Set per-event fake hit probability for each calorimeter in [0,1]");
    fCaloFakeProbabilityCmd->SetParameterName("prob", false);
    fCaloFakeProbabilityCmd->SetDefaultValue(fConfiguredCaloFakeProbability);
    fCaloFakeProbabilityCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fTriggerFakeProbabilityCmd = new G4UIcmdWithADouble("/art/triggerFakeProb", this);
    fTriggerFakeProbabilityCmd->SetGuidance("Set per-event fake trigger probability in [0,1]");
    fTriggerFakeProbabilityCmd->SetParameterName("prob", false);
    fTriggerFakeProbabilityCmd->SetDefaultValue(fConfiguredTriggerFakeProbability);
    fTriggerFakeProbabilityCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

ArtUICommands::~ArtUICommands()
{
    delete fAnalyzeCmd;
    delete fSaveCmd;
    delete fReportCmd;
    delete fBeamEnergyCmd;
    delete fBeamSigmaCmd;
    delete fBeamTiltYCmd;
    delete fGridCmd;
    delete fVerboseCmd;
    delete fInstrumentResponseCmd;
    delete fTrackerResolutionCmd;
    delete fCaloNoiseCmd;
    delete fCaloResolutionCmd;
    delete fTrackerEfficiencyCmd;
    delete fTriggerEfficiencyCmd;
    delete fCaloEfficiencyCmd;
    delete fTrackerFakeProbabilityCmd;
    delete fCaloFakeProbabilityCmd;
    delete fTriggerFakeProbabilityCmd;
    delete fArtDirectory;
}

void ArtUICommands::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == fAnalyzeCmd) {
        fAnalysisManager->AnalyzeLayers();
        fAnalysisManager->DetectHistoricalPigments();
        fAnalysisManager->CalculateScatteringSignatures();
        G4cout << "[art] analysis complete" << G4endl;
        return;
    }

    if (command == fSaveCmd) {
        G4String filename = newValue;
        if (filename.empty()) {
            filename = "bfs_results.csv";
        }
        fAnalysisManager->SaveResults(filename);
        G4cout << "[art] results written to " << filename << G4endl;
        return;
    }

    if (command == fReportCmd) {
        G4String format = newValue;
        if (format.empty() || format == "console" || format == "all") {
            fAnalysisManager->GenerateReport();
        }
        if (format == "file" || format == "all") {
            std::ofstream reportFile("bfs_report.txt");
            if (reportFile.is_open()) {
                std::streambuf* orig = std::cout.rdbuf();
                std::cout.rdbuf(reportFile.rdbuf());
                fAnalysisManager->GenerateReport();
                std::cout.rdbuf(orig);
                reportFile.close();
                G4cout << "[art] report written to bfs_report.txt" << G4endl;
            }
        }
        return;
    }

    if (command == fBeamEnergyCmd) {
        const G4double energy = fBeamEnergyCmd->GetNewDoubleValue(newValue);
        if (fPrimaryGenerator != nullptr) {
            fPrimaryGenerator->SetBeamEnergy(energy);
        }
        G4cout << "[art] beam energy set to " << energy / GeV << " GeV" << G4endl;
        return;
    }

    if (command == fBeamSigmaCmd) {
        const G4double sigma = fBeamSigmaCmd->GetNewDoubleValue(newValue);
        if (fPrimaryGenerator != nullptr) {
            fPrimaryGenerator->SetBeamSigma(sigma);
        }
        G4cout << "[art] beam sigma set to " << sigma / mm << " mm" << G4endl;
        return;
    }

    if (command == fBeamTiltYCmd) {
        const G4double tilt = fBeamTiltYCmd->GetNewDoubleValue(newValue);
        if (fPrimaryGenerator != nullptr) {
            const G4ThreeVector direction(0.0, std::sin(tilt), std::cos(tilt));
            fPrimaryGenerator->SetBeamDirection(direction);
        }
        G4cout << "[art] beam tilt Y set to " << tilt / mrad << " mrad" << G4endl;
        return;
    }

    if (command == fGridCmd) {
        const G4int n = G4UIcmdWithAnInteger::GetNewIntValue(newValue);
        fAnalysisManager->ConfigureGrid(n, n, 10.0 * cm, 10.0 * cm);
        G4cout << "[art] tomography grid set to " << n << "x" << n << G4endl;
        return;
    }

    if (command == fVerboseCmd) {
        const G4bool verbose = G4UIcmdWithABool::GetNewBoolValue(newValue);
        G4cout << "[art] verbose " << (verbose ? "enabled" : "disabled") << G4endl;
        return;
    }

    if (command == fInstrumentResponseCmd) {
        const G4bool enabled = G4UIcmdWithABool::GetNewBoolValue(newValue);
        if (fEventAction != nullptr) {
            fEventAction->SetInstrumentResponseEnabled(enabled);
        }
        G4cout << "[art] instrument response " << (enabled ? "enabled" : "disabled") << G4endl;
        return;
    }

    if (command == fTrackerResolutionCmd) {
        const G4double sigma = fTrackerResolutionCmd->GetNewDoubleValue(newValue);
        if (fEventAction != nullptr) {
            fEventAction->SetTrackerPositionResolution(sigma);
        }
        G4cout << "[art] tracker sigma set to " << sigma / micrometer << " um" << G4endl;
        return;
    }

    if (command == fCaloNoiseCmd) {
        const G4double noise = fCaloNoiseCmd->GetNewDoubleValue(newValue);
        fConfiguredCaloNoiseTerm = noise;
        if (fEventAction != nullptr) {
            fEventAction->SetCalorimeterResolution(
                fConfiguredCaloStochasticTerm, fConfiguredCaloConstantTerm, fConfiguredCaloNoiseTerm);
        }
        G4cout << "[art] calorimeter noise term set to " << noise / MeV << " MeV" << G4endl;
        return;
    }

    if (command == fCaloResolutionCmd) {
        std::istringstream iss(newValue);
        G4double stochastic = 0.10;
        G4double constant = 0.01;
        iss >> stochastic >> constant;
        fConfiguredCaloStochasticTerm = stochastic;
        fConfiguredCaloConstantTerm = constant;
        if (fEventAction != nullptr) {
            fEventAction->SetCalorimeterResolution(
                fConfiguredCaloStochasticTerm, fConfiguredCaloConstantTerm, fConfiguredCaloNoiseTerm);
        }
        G4cout << "[art] calorimeter terms set to stochastic=" << stochastic
               << " constant=" << constant << G4endl;
        return;
    }

    if (command == fTrackerEfficiencyCmd) {
        const G4double eff = ArtAnalysisManager::ClampProbability(fTrackerEfficiencyCmd->GetNewDoubleValue(newValue));
        if (fEventAction != nullptr) {
            fEventAction->SetTrackerEfficiency(eff);
        }
        G4cout << "[art] tracker efficiency set to " << eff << G4endl;
        return;
    }

    if (command == fTriggerEfficiencyCmd) {
        const G4double eff = ArtAnalysisManager::ClampProbability(fTriggerEfficiencyCmd->GetNewDoubleValue(newValue));
        if (fEventAction != nullptr) {
            fEventAction->SetTriggerEfficiency(eff);
        }
        G4cout << "[art] trigger efficiency set to " << eff << G4endl;
        return;
    }

    if (command == fCaloEfficiencyCmd) {
        const G4double eff = ArtAnalysisManager::ClampProbability(fCaloEfficiencyCmd->GetNewDoubleValue(newValue));
        if (fEventAction != nullptr) {
            fEventAction->SetCalorimeterEfficiency(eff);
        }
        G4cout << "[art] calorimeter efficiency set to " << eff << G4endl;
        return;
    }

    if (command == fTrackerFakeProbabilityCmd) {
        fConfiguredTrackerFakeProbability = ArtAnalysisManager::ClampProbability(
            fTrackerFakeProbabilityCmd->GetNewDoubleValue(newValue));
        if (fEventAction != nullptr) {
            fEventAction->SetFakeHitProbabilities(
                fConfiguredTrackerFakeProbability, fConfiguredCaloFakeProbability, fConfiguredTriggerFakeProbability);
        }
        G4cout << "[art] tracker fake probability set to " << fConfiguredTrackerFakeProbability << G4endl;
        return;
    }

    if (command == fCaloFakeProbabilityCmd) {
        fConfiguredCaloFakeProbability = ArtAnalysisManager::ClampProbability(
            fCaloFakeProbabilityCmd->GetNewDoubleValue(newValue));
        if (fEventAction != nullptr) {
            fEventAction->SetFakeHitProbabilities(
                fConfiguredTrackerFakeProbability, fConfiguredCaloFakeProbability, fConfiguredTriggerFakeProbability);
        }
        G4cout << "[art] calorimeter fake probability set to " << fConfiguredCaloFakeProbability << G4endl;
        return;
    }

    if (command == fTriggerFakeProbabilityCmd) {
        fConfiguredTriggerFakeProbability = ArtAnalysisManager::ClampProbability(
            fTriggerFakeProbabilityCmd->GetNewDoubleValue(newValue));
        if (fEventAction != nullptr) {
            fEventAction->SetFakeHitProbabilities(
                fConfiguredTrackerFakeProbability, fConfiguredCaloFakeProbability, fConfiguredTriggerFakeProbability);
        }
        G4cout << "[art] trigger fake probability set to " << fConfiguredTriggerFakeProbability << G4endl;
        return;
    }
}
