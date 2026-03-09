#include "ArtPrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4PhysicalConstants.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <cmath>

ArtPrimaryGeneratorAction::ArtPrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(),
      fBeamEnergy(2.0 * GeV),
      fBeamPosition(G4ThreeVector(0, 0, -40 * cm)),
      fBeamDirection(G4ThreeVector(0, 0, 1)),
      fBeamSigma(1.0 * mm)
{
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("e-");

    fParticleGun = new G4ParticleGun(1);
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(fBeamEnergy);
    fParticleGun->SetParticlePosition(fBeamPosition);
    fParticleGun->SetParticleMomentumDirection(fBeamDirection);
}

ArtPrimaryGeneratorAction::~ArtPrimaryGeneratorAction()
{
    delete fParticleGun;
}

void ArtPrimaryGeneratorAction::SetBeamEnergy(G4double energy)
{
    if (energy <= 0.0) {
        return;
    }
    fBeamEnergy = energy;
    fParticleGun->SetParticleEnergy(fBeamEnergy);
}

void ArtPrimaryGeneratorAction::SetBeamPosition(const G4ThreeVector& pos)
{
    fBeamPosition = pos;
    fParticleGun->SetParticlePosition(fBeamPosition);
}

void ArtPrimaryGeneratorAction::SetBeamDirection(const G4ThreeVector& dir)
{
    if (dir.mag2() <= 0.0) {
        return;
    }
    fBeamDirection = dir.unit();
    fParticleGun->SetParticleMomentumDirection(fBeamDirection);
}

void ArtPrimaryGeneratorAction::SetBeamSigma(G4double sigma)
{
    fBeamSigma = std::max(0.0, sigma);
}

void ArtPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    const G4double x = G4RandGauss::shoot(0.0, fBeamSigma);
    const G4double y = G4RandGauss::shoot(0.0, fBeamSigma);
    fParticleGun->SetParticlePosition(G4ThreeVector(fBeamPosition.x() + x, fBeamPosition.y() + y, fBeamPosition.z()));

    const G4double theta = G4RandGauss::shoot(0.0, 0.2 * mrad);
    const G4double phi = G4UniformRand() * 2.0 * CLHEP::pi;

    const G4ThreeVector axis = fBeamDirection.unit();
    const G4ThreeVector ortho1 = axis.orthogonal().unit();
    const G4ThreeVector ortho2 = axis.cross(ortho1).unit();
    G4ThreeVector direction = std::cos(theta) * axis + std::sin(theta) * (std::cos(phi) * ortho1 + std::sin(phi) * ortho2);
    direction = direction.unit();

    fParticleGun->SetParticleMomentumDirection(direction);
    fParticleGun->GeneratePrimaryVertex(event);
}
