# Guide Technique de la Simulation d'Analyse d'Art

## 📋 Table des Matières

1. [Architecture du Code](#architecture-du-code)
2. [Configuration Matérielle](#configuration-matérielle)
3. [Paramètres de Simulation](#paramètres-de-simulation)
4. [Algorithmes d'Analyse](#algorithmes-danalyse)
5. [Dépannage](#dépannage)
6. [Extensions Possibles](#extensions-possibles)

## 🏗️ Architecture du Code

### Structure des Classes

```
ArtAnalysisSimulation (main)
├── ArtDetectorConstruction
│   ├── DefineMaterials()
│   ├── SetupPaintLayers()
│   ├── ConstructWorld()
│   ├── ConstructArtwork()
│   └── ConstructDetectors()
├── ArtPhysicsList
├── ArtPrimaryGeneratorAction
├── ArtEventAction
├── ArtTrackingAction
├── ArtSteppingAction
└── ArtAnalysisManager
    ├── AnalyzeLayers()
    ├── DetectHistoricalPigments()
    ├── CalculateScatteringSignatures()
    └── GenerateReport()
```

### Flux de Données

```
Électrons → Trackers → Calorimètres → Analyse Manager → Rapports
    ↓           ↓            ↓              ↓            ↓
Position   Déviation   Énergie      Algorithmes   Visualisation
```

## 🎨 Configuration Matérielle

### Pigments Historiques Disponibles

| Nom | Formule Chimique | Z | Densité (g/cm³) | Époque Typique |
|-----|------------------|---|------------------|----------------|
| Blanc de plomb | 2PbCO₃·Pb(OH)₂ | 82 | 6.5 | Antiquité-1900 |
| Vermillon | HgS | 80 | 8.1 | Renaissance-XXème |
| Lapis-lazuli | Na₈₋₁₀Al₆Si₆O₂₄S₂₋₄ | 14 | 2.4 | Moyen Âge-Renaissance |
| Malachite | Cu₂CO₃(OH)₂ | 29 | 4.0 | Antiquité-Renaissance |
| Jaune de plomb-étain | Pb₂SnO₄ | 50 | 6.8 | XIVème-XVIIIème |

### Ajouter un Nouveau Pigment

1. **Déclarer le matériau** dans `ArtDetectorConstruction.hh`:
```cpp
G4Material* fNewPigmentMaterial;
```

2. **Définir le matériau** dans `DefineMaterials()`:
```cpp
fNewPigmentMaterial = new G4Material("NewPigment", density, nElements);
fNewPigmentMaterial->AddElement(nist->FindOrBuildElement("Element"), fraction);
```

3. **Ajouter la couche** dans `SetupPaintLayers()`:
```cpp
fPaintLayers.push_back({thickness, fNewPigmentMaterial, "NewPigment"});
```

4. **Mettre à jour l'analyse** dans `ArtAnalysisManager.cc`:
```cpp
if (material.contains("NewPigment")) return Z_value;
```

## ⚙️ Paramètres de Simulation

### Faisceau d'Électrons

| Paramètre | Plage Recommandée | Effet |
|-----------|-------------------|--------|
| Énergie | 50-200 keV | Pénétration vs sensibilité |
| Nombre d'électrons | 1000-100000 | Précision statistique |
| Diamètre | 1-10 mm | Résolution spatiale |
| Divergence | < 1 mrad | Focalisation |

### Détecteurs

#### Trackers
- **Position optimale**: ±15 cm de l'œuvre
- **Résolution**: 0.5 mm d'épaisseur
- **Surface**: 10×10 cm pour couverture complète

#### Calorimètres
- **Position optimale**: ±12 cm de l'œuvre
- **Résolution énergétique**: 1 keV
- **Épaisseur**: 1 cm pour arrêt complet

## 🔬 Algorithmes d'Analyse

### 1. Détection par Numéro Atomique

```cpp
G4double ArtAnalysisManager::CalculateMaterialZ(const G4String& material) {
    if (material.contains("Lead")) return 82;    // Plomb
    if (material.contains("Vermilion")) return 80; // Mercure
    if (material.contains("Lapis")) return 14;    // Sodium
    // ...
}
```

### 2. Calcul d'Épaisseur par Spectrométrie

```cpp
G4double ArtAnalysisManager::EstimateThickness(G4double energyLoss, G4double materialZ) {
    // Formule de Bethe simplifiée
    G4double stoppingPower = CalculateStoppingPower(material, 100*keV);
    return energyLoss / stoppingPower;
}
```

### 3. Analyse de Diffusion Multiple

```cpp
G4double expectedMS = 13.6*MeV * sqrt(thickness/(10*micrometer)) / 
                      (100*MeV) * (1 + 0.038*log(thickness/(10*micrometer)));
```

## 🐛 Dépannage

### Problèmes Communs

#### Erreur de Compilation
```
CMake Error: Could not find Geant4
```
**Solution**: Vérifiez les variables d'environnement:
```powershell
$env:GEANT4_DIR = "C:\path\to\geant4\lib\cmake\Geant4"
```

#### Segmentation Fault
**Causes possibles**:
- Pointeur non initialisé
- Accès mémoire invalide
- Matériau non défini

**Débogage**: Activez le mode debug:
```cpp
/run/verbose 2
/tracking/verbose 1
```

#### Résultats Incohérents
**Vérifications**:
1. Nombre d'électrons suffisant (> 5000)
2. Énergie du faisceau appropriée
3. Géométrie correcte des détecteurs

### Messages d'Erreur Spécifiques

#### "Material not found"
```cpp
// Vérifiez que le matériau est bien défini dans DefineMaterials()
if (!fNewMaterial) {
    G4Exception("ArtDetectorConstruction::DefineMaterials()", 
                "MatNotFound", FatalException, 
                "Material definition failed");
}
```

#### "Geometry overlap"
```cpp
// Vérifiez les positions des volumes
G4bool overlap = false;
G4VPhysicalVolume* pv = // ... votre volume
overlap = pv->CheckOverlaps();
```

## 🚀 Extensions Possibles

### 1. Machine Learning pour Classification

```python
# Entraînement sur des données de référence
from sklearn.ensemble import RandomForestClassifier

model = RandomForestClassifier(n_estimators=100)
model.fit(X_train, y_train)  # X: [Z, densité, épaisseur, énergie]
```

### 2. Base de Données de Pigments

```cpp
class PigmentDatabase {
private:
    struct PigmentInfo {
        G4String name;
        G4String formula;
        G4double Z;
        G4double density;
        G4String period;
        G4String region;
    };
    std::map<G4String, PigmentInfo> fDatabase;
};
```

### 3. Reconstruction 3D par Tomographie

```cpp
class TomographyReconstructor {
public:
    void AddProjection(const Projection& proj);
    G4ThreeVector ReconstructPoint(G4int x, G4int y);
    void Generate3DModel();
};
```

### 4. Interface Graphique Avancée

```cpp
// Qt-based GUI
class ArtAnalysisGUI : public QMainWindow {
    Q_OBJECT
public:
    void SetBeamParameters(double energy, int electrons);
    void StartSimulation();
    void DisplayResults(const AnalysisResults& results);
};
```

## 📊 Performance et Optimisation

### Optimisations Recommandées

1. **Parallélisation**: Utilisez `G4MTRunManager`
2. **Événements**: Limitez le verbose en production
3. **Mémoire**: Nettoyez les données entre les runs
4. **Physique**: Désactivez les processus inutiles

### Benchmarks

| Nombre d'électrons | Temps (s) | Mémoire (MB) |
|-------------------|------------|---------------|
| 1,000 | 2 | 50 |
| 10,000 | 15 | 120 |
| 100,000 | 180 | 800 |

## 🔗 Références Externes

- **Geant4 Documentation**: https://geant4.web.cern.ch/
- **Bethe-Bloch Formula**: https://en.wikipedia.org/wiki/Bethe_formula
- **Pigments History**: https://www.nationalgallery.org.uk/paintings/
- **Electron Scattering**: https://pdg.lbl.gov/

## 📞 Support Technique

Pour toute question technique:
1. Consultez ce guide
2. Vérifiez les logs de compilation
3. Testez avec les données de validation
4. Contactez le développeur principal

---

*Dernière mise à jour: Décembre 2024*
