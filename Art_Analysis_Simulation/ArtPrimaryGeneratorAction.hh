#ifndef ArtPrimaryGeneratorAction_h
#define ArtPrimaryGeneratorAction_h 1

#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

class ArtPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    ArtPrimaryGeneratorAction();
    virtual ~ArtPrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event* event) override;

    void SetBeamEnergy(G4double energy);
    void SetBeamPosition(const G4ThreeVector& pos);
    void SetBeamDirection(const G4ThreeVector& dir);
    void SetBeamSigma(G4double sigma);

    G4double GetBeamEnergy() const { return fBeamEnergy; }
    const G4ThreeVector& GetBeamPosition() const { return fBeamPosition; }
    const G4ThreeVector& GetBeamDirection() const { return fBeamDirection; }
    G4double GetBeamSigma() const { return fBeamSigma; }

private:
    G4ParticleGun* fParticleGun;
    G4double fBeamEnergy;
    G4ThreeVector fBeamPosition;
    G4ThreeVector fBeamDirection;
    G4double fBeamSigma;
};

#endif
