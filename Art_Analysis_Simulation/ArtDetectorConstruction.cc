#include "ArtDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"

#include <cmath>
#include <map>

ArtDetectorConstruction::ArtDetectorConstruction()
: G4VUserDetectorConstruction(),
  fWorldPhysical(nullptr),
  fWorldLogical(nullptr),
  fArtworkLogical(nullptr),
  fArtworkPhysical(nullptr),
  fTracker1Logical(nullptr),
  fTracker2Logical(nullptr),
  fTriggerLogical(nullptr),
  fCalorimeter1Logical(nullptr),
  fCalorimeter2Logical(nullptr),
  fArtworkWidth(20*cm),
  fArtworkHeight(20*cm),
  fArtworkDepth(2*mm),
  fArtworkShiftX(0.0),
  fArtworkShiftY(0.0),
  fArtworkTiltY(0.0)
{
    DefineMaterials();
}

ArtDetectorConstruction::~ArtDetectorConstruction()
{
}

void ArtDetectorConstruction::DefineMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();
    
    // Air
    fAirMaterial = nist->FindOrBuildMaterial("G4_AIR");
    
    // Blanc de plomb (2PbCO3·Pb(OH)2) - peinture ancienne
    // Approximation: mélange de plomb, carbone, oxygène, hydrogène
    fLeadWhiteMaterial = new G4Material("LeadWhite", 6.5*g/cm3, 4);
    fLeadWhiteMaterial->AddElement(nist->FindOrBuildElement("Pb"), 0.75);
    fLeadWhiteMaterial->AddElement(nist->FindOrBuildElement("C"), 0.08);
    fLeadWhiteMaterial->AddElement(nist->FindOrBuildElement("O"), 0.15);
    fLeadWhiteMaterial->AddElement(nist->FindOrBuildElement("H"), 0.02);
    
    // Peinture moderne (dioxyde de titane + liants organiques)
    fModernPaintMaterial = new G4Material("ModernPaint", 2.0*g/cm3, 4);
    fModernPaintMaterial->AddElement(nist->FindOrBuildElement("Ti"), 0.30);
    fModernPaintMaterial->AddElement(nist->FindOrBuildElement("C"), 0.40);
    fModernPaintMaterial->AddElement(nist->FindOrBuildElement("O"), 0.25);
    fModernPaintMaterial->AddElement(nist->FindOrBuildElement("H"), 0.05);
    
    // Toile (coton)
    fCanvasMaterial = new G4Material("Canvas", 1.5*g/cm3, 3);
    fCanvasMaterial->AddElement(nist->FindOrBuildElement("C"), 0.60);
    fCanvasMaterial->AddElement(nist->FindOrBuildElement("H"), 0.10);
    fCanvasMaterial->AddElement(nist->FindOrBuildElement("O"), 0.30);
    
    // Gesso (préparation)
    fGessoMaterial = new G4Material("Gesso", 1.8*g/cm3, 4);
    fGessoMaterial->AddElement(nist->FindOrBuildElement("Ca"), 0.20);
    fGessoMaterial->AddElement(nist->FindOrBuildElement("C"), 0.15);
    fGessoMaterial->AddElement(nist->FindOrBuildElement("O"), 0.55);
    fGessoMaterial->AddElement(nist->FindOrBuildElement("H"), 0.10);
    
    // Vernis
    fVarnishMaterial = new G4Material("Varnish", 1.2*g/cm3, 3);
    fVarnishMaterial->AddElement(nist->FindOrBuildElement("C"), 0.70);
    fVarnishMaterial->AddElement(nist->FindOrBuildElement("H"), 0.10);
    fVarnishMaterial->AddElement(nist->FindOrBuildElement("O"), 0.20);
    
    // Silicium pour détecteurs
    fSiliconMaterial = nist->FindOrBuildMaterial("G4_Si");
    fScintillatorMaterial = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    fEcalMaterial = nist->FindOrBuildMaterial("G4_PbWO4");
    if (fEcalMaterial == nullptr) {
        fEcalMaterial = nist->FindOrBuildMaterial("G4_BGO");
    }
    
    // Vermillon (HgS) - pigment rouge historique très dense
    fVermilionMaterial = new G4Material("Vermilion", 8.1*g/cm3, 2);
    fVermilionMaterial->AddElement(nist->FindOrBuildElement("Hg"), 0.86);
    fVermilionMaterial->AddElement(nist->FindOrBuildElement("S"), 0.14);
    
    // Lapis-lazuli (Na8-10Al6Si6O24S2-4) - pigment bleu historique précieux
    fLapisLazuliMaterial = new G4Material("LapisLazuli", 2.4*g/cm3, 5);
    fLapisLazuliMaterial->AddElement(nist->FindOrBuildElement("Na"), 0.10);
    fLapisLazuliMaterial->AddElement(nist->FindOrBuildElement("Al"), 0.15);
    fLapisLazuliMaterial->AddElement(nist->FindOrBuildElement("Si"), 0.20);
    fLapisLazuliMaterial->AddElement(nist->FindOrBuildElement("O"), 0.45);
    fLapisLazuliMaterial->AddElement(nist->FindOrBuildElement("S"), 0.10);
    
    // Malachite (Cu2CO3(OH)2) - pigment vert historique
    fMalachiteMaterial = new G4Material("Malachite", 4.0*g/cm3, 4);
    fMalachiteMaterial->AddElement(nist->FindOrBuildElement("Cu"), 0.57);
    fMalachiteMaterial->AddElement(nist->FindOrBuildElement("C"), 0.06);
    fMalachiteMaterial->AddElement(nist->FindOrBuildElement("O"), 0.31);
    fMalachiteMaterial->AddElement(nist->FindOrBuildElement("H"), 0.06);
    
    // Jaune de plomb-étain (Pb2SnO4) - pigment jaune historique
    fLeadTinYellowMaterial = new G4Material("LeadTinYellow", 6.8*g/cm3, 3);
    fLeadTinYellowMaterial->AddElement(nist->FindOrBuildElement("Pb"), 0.59);
    fLeadTinYellowMaterial->AddElement(nist->FindOrBuildElement("Sn"), 0.29);
    fLeadTinYellowMaterial->AddElement(nist->FindOrBuildElement("O"), 0.12);
}

void ArtDetectorConstruction::SetupPaintLayers()
{
    fPaintLayers.clear();
    
    // Configuration avancée: œuvre maîtresse avec multiples couches historiques
    // Couche 1: Vernis de protection (30 μm)
    fPaintLayers.push_back({30*micrometer, fVarnishMaterial, "Varnish"});
    
    // Couche 2: Restaurations modernes (80 μm)
    fPaintLayers.push_back({80*micrometer, fModernPaintMaterial, "ModernRestoration"});
    
    // Couche 3: Détails en vermillon - pigment historique dense (60 μm)
    fPaintLayers.push_back({60*micrometer, fVermilionMaterial, "Vermilion"});
    
    // Couche 4: Fond en lapis-lazuli - pigment historique précieux (150 μm)
    fPaintLayers.push_back({150*micrometer, fLapisLazuliMaterial, "LapisLazuli"});
    
    // Couche 5: Blanc de plomb - couche principale (200 μm)
    fPaintLayers.push_back({200*micrometer, fLeadWhiteMaterial, "LeadWhite"});
    
    // Couche 6: Touches de malachite - accents verts (40 μm)
    fPaintLayers.push_back({40*micrometer, fMalachiteMaterial, "Malachite"});
    
    // Couche 7: Jaune de plomb-étain - détails dorés (50 μm)
    fPaintLayers.push_back({50*micrometer, fLeadTinYellowMaterial, "LeadTinYellow"});
    
    // Couche 8: Gesso - préparation traditionnelle (500 μm)
    fPaintLayers.push_back({500*micrometer, fGessoMaterial, "Gesso"});
    
    // Couche 9: Toile (1 mm)
    fPaintLayers.push_back({1*mm, fCanvasMaterial, "Canvas"});
}

void ArtDetectorConstruction::ConstructWorld()
{
    // World volume
    G4double worldSize = 120*cm;
    G4Box* solidWorld = new G4Box("World", worldSize/2, worldSize/2, worldSize/2);
    fWorldLogical = new G4LogicalVolume(solidWorld, fAirMaterial, "World");
    
    fWorldPhysical = new G4PVPlacement(0, G4ThreeVector(), fWorldLogical, "World", 0, false, 0, true);
}

void ArtDetectorConstruction::ConstructArtwork()
{
    SetupPaintLayers();
    
    // Calculate total thickness
    G4double totalThickness = 0;
    for (const auto& layer : fPaintLayers) {
        totalThickness += layer.thickness;
    }
    
    fArtworkDepth = totalThickness;

    // Envelope that can be moved/tilted to emulate mechanical scans
    G4Box* solidArtwork = new G4Box("ArtworkEnvelope", fArtworkWidth / 2, fArtworkHeight / 2, totalThickness / 2);
    fArtworkLogical = new G4LogicalVolume(solidArtwork, fAirMaterial, "ArtworkEnvelope");

    G4RotationMatrix* artworkRotation = new G4RotationMatrix();
    artworkRotation->rotateX(fArtworkTiltY);
    fArtworkPhysical = new G4PVPlacement(
        artworkRotation,
        G4ThreeVector(fArtworkShiftX, fArtworkShiftY, 0.0),
        fArtworkLogical,
        "ArtworkEnvelope",
        fWorldLogical,
        false,
        0,
        true);

    // Create artwork as stacked layers inside the envelope
    G4double currentZ = -totalThickness/2;
    std::map<G4String, G4LogicalVolume*> layerLogicalByName;
    
    for (size_t i = 0; i < fPaintLayers.size(); ++i) {
        const auto& layer = fPaintLayers[i];
        
        G4Box* solidLayer = new G4Box(
            layer.name,
            fArtworkWidth/2,
            fArtworkHeight/2,
            layer.thickness/2
        );
        
        G4LogicalVolume* logicalLayer = new G4LogicalVolume(
            solidLayer,
            layer.material,
            layer.name
        );
        
        // Position layer
        G4ThreeVector position(0, 0, currentZ + layer.thickness/2);
        
        new G4PVPlacement(
            0,
            position,
            logicalLayer,
            layer.name,
            fArtworkLogical,
            false,
            static_cast<G4int>(i),
            true
        );
        
        // Visualization attributes
        G4VisAttributes* visAtt = new G4VisAttributes();
        if (layer.material == fLeadWhiteMaterial) {
            visAtt->SetColour(G4Colour(0.8, 0.8, 0.8)); // Light gray for lead white
        } else if (layer.material == fModernPaintMaterial) {
            visAtt->SetColour(G4Colour(0.6, 0.3, 0.1)); // Brown for modern paint
        } else if (layer.material == fVarnishMaterial) {
            visAtt->SetColour(G4Colour(1.0, 1.0, 0.8)); // Yellowish for varnish
        } else if (layer.material == fVermilionMaterial) {
            visAtt->SetColour(G4Colour(0.9, 0.1, 0.1)); // Red for vermillion
        } else if (layer.material == fLapisLazuliMaterial) {
            visAtt->SetColour(G4Colour(0.1, 0.2, 0.8)); // Deep blue for lapis-lazuli
        } else if (layer.material == fMalachiteMaterial) {
            visAtt->SetColour(G4Colour(0.1, 0.6, 0.3)); // Green for malachite
        } else if (layer.material == fLeadTinYellowMaterial) {
            visAtt->SetColour(G4Colour(0.9, 0.7, 0.2)); // Golden yellow for lead-tin yellow
        } else {
            visAtt->SetColour(G4Colour(0.9, 0.9, 0.9)); // Light gray for others
        }
        visAtt->SetVisibility(true);
        visAtt->SetForceSolid(true);
        logicalLayer->SetVisAttributes(visAtt);
        layerLogicalByName[layer.name] = logicalLayer;
        
        currentZ += layer.thickness;
    }

    // Non-uniform lateral structures: hidden inclusions with different materials.
    auto placeInclusion = [&](const G4String& name,
                              const G4String& layerName,
                              G4Material* material,
                              G4double sizeX,
                              G4double sizeY,
                              G4double sizeZ,
                              const G4ThreeVector& pos,
                              const G4Colour& color) {
        auto it = layerLogicalByName.find(layerName);
        if (it == layerLogicalByName.end() || material == nullptr) {
            return;
        }
        G4Box* solid = new G4Box(name + "Solid", sizeX / 2.0, sizeY / 2.0, sizeZ / 2.0);
        G4LogicalVolume* logical = new G4LogicalVolume(solid, material, name);
        new G4PVPlacement(
            0,
            pos,
            logical,
            name,
            it->second,
            false,
            0,
            true);
        G4VisAttributes* visAtt = new G4VisAttributes(color);
        visAtt->SetVisibility(true);
        visAtt->SetForceSolid(true);
        logical->SetVisAttributes(visAtt);
    };

    placeInclusion(
        "HiddenVermilionPatch",
        "LeadWhite",
        fVermilionMaterial,
        3.5 * cm,
        2.0 * cm,
        120 * micrometer,
        G4ThreeVector(-3.0 * cm, 2.5 * cm, 0.0),
        G4Colour(0.95, 0.2, 0.2));
    placeInclusion(
        "HiddenLeadStripe",
        "Gesso",
        fLeadTinYellowMaterial,
        7.0 * cm,
        0.9 * cm,
        220 * micrometer,
        G4ThreeVector(2.8 * cm, -2.0 * cm, 0.0),
        G4Colour(0.9, 0.75, 0.2));
    placeInclusion(
        "RestorationWindow",
        "LapisLazuli",
        fModernPaintMaterial,
        2.8 * cm,
        2.8 * cm,
        100 * micrometer,
        G4ThreeVector(1.0 * cm, 3.5 * cm, 0.0),
        G4Colour(0.55, 0.35, 0.2));

    G4VisAttributes* artworkVis = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.08));
    artworkVis->SetVisibility(true);
    artworkVis->SetForceWireframe(true);
    fArtworkLogical->SetVisAttributes(artworkVis);
}

void ArtDetectorConstruction::ConstructDetectors()
{
    // Trigger scintillator
    G4Box* solidTrigger = new G4Box("TriggerScintillator", 12*cm, 12*cm, 0.5*mm);
    fTriggerLogical = new G4LogicalVolume(solidTrigger, fScintillatorMaterial, "TriggerScintillator");
    new G4PVPlacement(0, G4ThreeVector(0, 0, -35*cm), fTriggerLogical, "TriggerScintillator", fWorldLogical, false, 0, true);

    // Upstream tracking pair
    G4Box* solidTrackerUp1 = new G4Box("TrackerUp1", 12*cm, 12*cm, 0.5*mm);
    fTracker1Logical = new G4LogicalVolume(solidTrackerUp1, fSiliconMaterial, "TrackerUp1");
    new G4PVPlacement(0, G4ThreeVector(0, 0, -30*cm), fTracker1Logical, "TrackerUp1", fWorldLogical, false, 0, true);

    G4Box* solidTrackerUp2 = new G4Box("TrackerUp2", 12*cm, 12*cm, 0.5*mm);
    G4LogicalVolume* trackerUp2Logical = new G4LogicalVolume(solidTrackerUp2, fSiliconMaterial, "TrackerUp2");
    new G4PVPlacement(0, G4ThreeVector(0, 0, -20*cm), trackerUp2Logical, "TrackerUp2", fWorldLogical, false, 0, true);

    // Downstream tracking pair
    G4Box* solidTrackerDown1 = new G4Box("TrackerDown1", 12*cm, 12*cm, 0.5*mm);
    G4LogicalVolume* trackerDown1Logical = new G4LogicalVolume(solidTrackerDown1, fSiliconMaterial, "TrackerDown1");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 20*cm), trackerDown1Logical, "TrackerDown1", fWorldLogical, false, 0, true);

    G4Box* solidTrackerDown2 = new G4Box("TrackerDown2", 12*cm, 12*cm, 0.5*mm);
    fTracker2Logical = new G4LogicalVolume(solidTrackerDown2, fSiliconMaterial, "TrackerDown2");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 30*cm), fTracker2Logical, "TrackerDown2", fWorldLogical, false, 0, true);

    // Thin upstream calorimeter to estimate incoming energy near the object
    G4Box* solidCalorimeterIn = new G4Box("CalorimeterIn", 12*cm, 12*cm, 0.25*mm);
    fCalorimeter1Logical = new G4LogicalVolume(solidCalorimeterIn, fSiliconMaterial, "CalorimeterIn");
    new G4PVPlacement(0, G4ThreeVector(0, 0, -10*cm), fCalorimeter1Logical, "CalorimeterIn", fWorldLogical, false, 0, true);

    // Dense ECAL-like block for outgoing-energy measurement
    G4Box* solidCalorimeterOut = new G4Box("CalorimeterOut", 12*cm, 12*cm, 5*cm);
    fCalorimeter2Logical = new G4LogicalVolume(solidCalorimeterOut, fEcalMaterial, "CalorimeterOut");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 40*cm), fCalorimeter2Logical, "CalorimeterOut", fWorldLogical, false, 0, true);
    
    // Visualization for detectors
    G4VisAttributes* trackerVis = new G4VisAttributes(G4Colour(0.0, 0.8, 0.0, 0.7)); // Green, semi-transparent
    trackerVis->SetVisibility(true);
    fTracker1Logical->SetVisAttributes(trackerVis);
    trackerUp2Logical->SetVisAttributes(trackerVis);
    trackerDown1Logical->SetVisAttributes(trackerVis);
    fTracker2Logical->SetVisAttributes(trackerVis);

    G4VisAttributes* triggerVis = new G4VisAttributes(G4Colour(0.1, 0.7, 0.9, 0.6));
    triggerVis->SetVisibility(true);
    fTriggerLogical->SetVisAttributes(triggerVis);
    
    G4VisAttributes* calorimeterVis = new G4VisAttributes(G4Colour(0.8, 0.0, 0.0, 0.7)); // Red, semi-transparent
    calorimeterVis->SetVisibility(true);
    fCalorimeter1Logical->SetVisAttributes(calorimeterVis);
    fCalorimeter2Logical->SetVisAttributes(calorimeterVis);
}

G4VPhysicalVolume* ArtDetectorConstruction::Construct()
{
    ConstructWorld();
    ConstructArtwork();
    ConstructDetectors();
    
    return fWorldPhysical;
}

void ArtDetectorConstruction::SetArtworkShiftXY(G4double x, G4double y)
{
    fArtworkShiftX = x;
    fArtworkShiftY = y;
}

void ArtDetectorConstruction::SetArtworkTiltY(G4double angle)
{
    fArtworkTiltY = angle;
}

G4ThreeVector ArtDetectorConstruction::WorldToArtworkLocal(const G4ThreeVector& worldPoint) const
{
    // Inverse transform of envelope placement:
    // world = R_x(tilt) * local + shift.
    const G4ThreeVector shifted = worldPoint - G4ThreeVector(fArtworkShiftX, fArtworkShiftY, 0.0);
    const G4double c = std::cos(fArtworkTiltY);
    const G4double s = std::sin(fArtworkTiltY);
    return G4ThreeVector(
        shifted.x(),
        c * shifted.y() + s * shifted.z(),
        -s * shifted.y() + c * shifted.z());
}
