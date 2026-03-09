#ifndef ArtDetectorConstruction_h
#define ArtDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"

class ArtDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    ArtDetectorConstruction();
    virtual ~ArtDetectorConstruction();
    
    virtual G4VPhysicalVolume* Construct() override;
    
private:
    // Materials
    void DefineMaterials();
    G4Material* fAirMaterial;
    G4Material* fLeadWhiteMaterial;    // Blanc de plomb (peinture ancienne)
    G4Material* fModernPaintMaterial;    // Peinture moderne
    G4Material* fCanvasMaterial;         // Support toile
    G4Material* fGessoMaterial;         // Préparation
    G4Material* fVarnishMaterial;       // Vernis
    G4Material* fSiliconMaterial;        // Détecteurs
    G4Material* fVermilionMaterial;      // Vermillon (HgS) - pigment historique
    G4Material* fLapisLazuliMaterial;    // Lapis-lazuli (Na8-10Al6Si6O24S2-4) - pigment historique
    G4Material* fMalachiteMaterial;      // Malachite (Cu2CO3(OH)2) - pigment historique
    G4Material* fLeadTinYellowMaterial;  // Jaune de plomb-étain (Pb2SnO4) - pigment historique
    G4Material* fScintillatorMaterial;   // Trigger scintillator
    
    // Geometry
    G4VPhysicalVolume* fWorldPhysical;
    G4LogicalVolume* fWorldLogical;
    G4LogicalVolume* fArtworkLogical;
    G4LogicalVolume* fTracker1Logical;
    G4LogicalVolume* fTracker2Logical;
    G4LogicalVolume* fTriggerLogical;
    G4LogicalVolume* fCalorimeter1Logical;
    G4LogicalVolume* fCalorimeter2Logical;
    
    // Artwork parameters
    G4double fArtworkWidth;
    G4double fArtworkHeight;
    G4double fArtworkDepth;
    
    // Layer structure
    struct PaintLayer {
        G4double thickness;
        G4Material* material;
        G4String name;
    };
    std::vector<PaintLayer> fPaintLayers;
    
    void ConstructWorld();
    void ConstructArtwork();
    void ConstructDetectors();
    void SetupPaintLayers();
};

#endif
