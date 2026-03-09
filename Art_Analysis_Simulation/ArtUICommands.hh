#ifndef ArtUICommands_h
#define ArtUICommands_h 1

#include "G4UIdirectory.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"

class ArtAnalysisManager;
class ArtPrimaryGeneratorAction;
class ArtEventAction;

class ArtUICommands : public G4UImessenger
{
public:
    ArtUICommands(ArtAnalysisManager* analysisManager, ArtPrimaryGeneratorAction* primaryGenerator, ArtEventAction* eventAction);
    virtual ~ArtUICommands();

    virtual void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
    ArtAnalysisManager* fAnalysisManager;
    ArtPrimaryGeneratorAction* fPrimaryGenerator;
    ArtEventAction* fEventAction;

    G4UIdirectory* fArtDirectory;

    G4UIcmdWithAString* fAnalyzeCmd;
    G4UIcmdWithAString* fSaveCmd;
    G4UIcmdWithAString* fReportCmd;
    G4UIcmdWithADoubleAndUnit* fBeamEnergyCmd;
    G4UIcmdWithADoubleAndUnit* fBeamSigmaCmd;
    G4UIcmdWithADoubleAndUnit* fBeamTiltYCmd;
    G4UIcmdWithAnInteger* fGridCmd;
    G4UIcmdWithABool* fVerboseCmd;
    G4UIcmdWithABool* fInstrumentResponseCmd;
    G4UIcmdWithADoubleAndUnit* fTrackerResolutionCmd;
    G4UIcmdWithADoubleAndUnit* fCaloNoiseCmd;
    G4UIcmdWithAString* fCaloResolutionCmd;
    G4UIcmdWithADouble* fTrackerEfficiencyCmd;
    G4UIcmdWithADouble* fTriggerEfficiencyCmd;
    G4UIcmdWithADouble* fCaloEfficiencyCmd;
    G4UIcmdWithADouble* fTrackerFakeProbabilityCmd;
    G4UIcmdWithADouble* fCaloFakeProbabilityCmd;
    G4UIcmdWithADouble* fTriggerFakeProbabilityCmd;
    G4double fConfiguredCaloStochasticTerm;
    G4double fConfiguredCaloConstantTerm;
    G4double fConfiguredCaloNoiseTerm;
    G4double fConfiguredTrackerFakeProbability;
    G4double fConfiguredCaloFakeProbability;
    G4double fConfiguredTriggerFakeProbability;
};

#endif
