# Simulation d'Analyse d'Œuvres d'Art par Faisceau d'Électrons

## Objectif

Développer une simulation Geant4 pour analyser des œuvres d'art de manière non invasive en utilisant un faisceau d'électrons.

## Principe de l'expérience

### Méthodologie
1. **Faisceau d'électrons** : Un faisceau collimaté d'électrons de 1-5 MeV
2. **Cible** : Œuvre d'art avec différentes couches de peinture
3. **Détection** : 
   - **Trackers** : Mesure de la déviation des électrons
   - **Calorimètres** : Mesure de l'énergie avant et après passage

### Applications attendues

#### 1. **Identification des pigments**
- **Peintures anciennes** : Pigments à base de plomb (forte déviation)
- **Peintures modernes** : Pigments organiques (faible déviation)
- **Analyse quantitative** : Cartographie de la composition

#### 2. **Mesure d'épaisseur des couches**
- **Principe** : Différence d'énergie ΔE = E_entrée - E_sortie
- **Précision** : Résolution sub-micrométrique possible
- **Profondeur** : Analyse couche par couche

#### 3. **Avantages de la méthode**
- **Non invasive** : Pas de prélèvement
- **Haute précision** : Résolution spatiale < 100 μm
- **Analyse 3D** : Profilage en profondeur
- **Multi-éléments** : Détection de Z élevés (plomb, mercure, etc.)

## Configuration de la simulation

### Paramètres du faisceau
- Énergie : 1-5 MeV (ajustable)
- Diamètre : 100 μm - 1 mm
- Intensité : 10^6 - 10^9 électrons/s
- Collimation : < 1 mrad

### Géométrie de détection
```
      Source d'électrons
             ↓
    ┌─────────────────┐
    │  Tracker 1     │ ← Position initiale
    │  (précision μm)│
    └─────────────────┘
             ↓
    ┌─────────────────┐
    │ Calorimètre 1   │ ← Énergie d'entrée
    │  (E_entrée)    │
    └─────────────────┘
             ↓
    ┌─────────────────┐
    │   Œuvre d'art  │ ← Cible analysée
    │  (couches)     │
    └─────────────────┘
             ↓
    ┌─────────────────┐
    │ Calorimètre 2   │ ← Énergie de sortie
    │  (E_sortie)    │
    └─────────────────┘
             ↓
    ┌─────────────────┐
    │  Tracker 2     │ ← Position finale
    │  (précision μm)│
    └─────────────────┘
```

### Matériaux à simuler
- **Support** : Toile, bois, mur
- **Préparation** : Gesso, enduit
- **Peintures anciennes** :
  - Blanc de plomb (2PbCO₃·Pb(OH)₂)
  - Vermillon (HgS)
  - Minium (Pb₃O₄)
- **Peintures modernes** :
  - Dioxyde de titane (TiO₂)
  - Pigments organiques
- **Vernis** : Résines naturelles/synthétiques

## Résultats attendus

### 1. **Déviation angulaire**
- **Forte déviation** : Z élevé (plomb, mercure)
- **Faible déviation** : Z faible (carbone, hydrogène)
- **Cartographie** : Distribution spatiale des pigments

### 2. **Perte d'énergie**
- **Relation** : dE/dx ∝ Z²
- **Épaisseur** : ΔE = ∫(dE/dx)dx
- **Résolution** : ~1 μm pour couches de peinture

### 3. **Analyse 3D**
- **Profondeur** : Balayage en énergie
- **Résolution latérale** : Taille du faisceau
- **Volume analysé** : 100×100×500 μm³

## Applications pratiques

### 1. **Authentification d'œuvres**
- Détection de pigments anachroniques
- Analyse des techniques de peinture
- Comparaison avec bases de données

### 2. **Conservation**
- Suivi de l'état de dégradation
- Détection de restaurations anciennes
- Planification des traitements

### 3. **Recherche**
- Étude des techniques anciennes
- Analyse des mélanges de pigments
- Cartographie des altérations

## Implémentation Geant4

### Structure du code
- `ArtDetectorConstruction.cc` : Géométrie de l'expérience
- `ArtPhysicsList.cc` : Processus physiques adaptés
- `ArtPrimaryGeneratorAction.cc` : Génération du faisceau
- `ArtTrackingAction.cc` : Analyse des trajectoires
- `ArtEventAction.cc` : Collecte des données

### Données de sortie
- Trajectoires des électrons
- Énergies déposées par couche
- Déviations angulaires
- Cartes 2D/3D de composition
