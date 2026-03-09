# Théorie de l'Expérience: Analyse d'Œuvres d'Art par Faisceau d'Électrons

## Principe Physique Fondamental

L'analyse d'œuvres d'art par faisceau d'électrons repose sur deux phénomènes physiques principaux:

### 1. **Diffusion Multiple Coulombienne**

Lorsqu'un électron traverse la matière, il subit des interactions électromagnétiques avec les noyaux atomiques:

**Force de Coulomb:**
```
F = k × (Z₁ × Z₂ × e²) / r²
```

Où:
- Z₁, Z₂: numéros atomiques
- e: charge élémentaire
- r: distance entre particules

**Angle de diffusion moyen:**
```
θ_rms ∝ Z × √(t/X₀)
```
Où:
- Z: numéro atomique du matériau
- t: épaisseur traversée
- X₀: longueur de radiation

### 2. **Perte d'Énergie par Ionisation**

Les électrons perdent de l'énergie en ionisant les atomes du matériau:

**Formule de Bethe-Bloch (simplifiée):**
```
dE/dx ∝ Z × ρ / (β² × γ²)
```
Où:
- Z: numéro atomique
- ρ: masse volumique
- β, γ: facteurs relativistes

## Application à l'Analyse d'Art

### **Identification des Pigments**

#### Peintures Anciennes (Base de Plomb)
- **Blanc de plomb**: 2PbCO₃·Pb(OH)₂
- **Z effectif**: ~75 (très élevé)
- **Caractéristiques**:
  - Forte diffusion (θ > 10 mrad)
  - Perte d'énergie élevée (dE/dx > 2 MeV/cm)
  - Forte rétrodiffusion

#### Peintures Modernes
- **Dioxyde de titane**: TiO₂
- **Pigments organiques**: C, H, O, N
- **Z effectif**: ~10-20
- **Caractéristiques**:
  - Faible diffusion (θ < 2 mrad)
  - Perte d'énergie modérée (dE/dx < 1 MeV/cm)

### **Mesure d'Épaisseur des Couches**

**Principe**: La différence d'énergie avant et après traversée donne l'épaisseur:

```
ΔE = ∫(dE/dx)dx
t = ΔE / (dE/dx)_moyen
```

**Précision attendue**:
- Résolution latérale: ~100 μm (diamètre du faisceau)
- Résolution en profondeur: ~1 μm (précision énergétique)
- Sensibilité en Z: ΔZ/Z < 5%

## Configuration Expérimentale Optimale

### **Paramètres du Faisceau**
- **Énergie**: 1-5 MeV (compromis pénétration/sensibilité)
- **Intensité**: 10⁶-10⁹ e⁻/s (éviter dommages)
- **Diamètre**: 100-500 μm (résolution spatiale)
- **Divergence**: < 1 mrad (focalisation précise)

### **Système de Détection**

#### Trackers (Position)
- **Technologie**: Silicium micro-strié
- **Précision**: ~1 μm
- **Temps de réponse**: < 10 ns
- **Position**: ±10 cm de l'œuvre

#### Calorimètres (Énergie)
- **Technologie**: Scintillateur + photodiode
- **Résolution énergétique**: ΔE/E < 1%
- **Gamme**: 0.1-10 MeV
- **Position**: ±8 cm de l'œuvre

## Analyse des Données

### **Traitement des Trajectoires**

```python
# Pseudo-code pour l'analyse
def analyze_track(track):
    initial_pos = track.position_at_tracker1
    final_pos = track.position_at_tracker2
    
    # Calcul de la déviation
    deviation = calculate_deviation(initial_pos, final_pos)
    
    # Calcul de la perte d'énergie
    energy_loss = track.energy_cal1 - track.energy_cal2
    
    # Identification du matériau
    material = identify_material(deviation, energy_loss)
    
    return material, energy_loss, deviation
```

### **Cartographie 3D**

1. **Balayage latéral**: Déplacement du faisceau en X-Y
2. **Analyse en profondeur**: Variation de l'énergie du faisceau
3. **Reconstruction**: Algorithmes de tomographie

### **Bases de Données de Référence**

| Matériau | Z effectif | dE/dx (MeV/cm) | θ_rms (mrad/μm) |
|-----------|------------|------------------|------------------|
| Blanc de plomb | 75 | 2.8 | 0.15 |
| Vermillon (HgS) | 80 | 3.1 | 0.18 |
| Minium (Pb₃O₄) | 70 | 2.5 | 0.12 |
| Dioxyde de titane | 22 | 0.8 | 0.03 |
| Carbone organique | 6 | 0.2 | 0.008 |
| Toile (coton) | 7 | 0.3 | 0.01 |

## Applications Pratiques

### **Authentification**
- Détection de pigments anachroniques
- Identification de faux
- Datation relative des couches

### **Conservation**
- Suivi de l'état de dégradation
- Détection de zones restaurées
- Planification des traitements

### **Recherche**
- Étude des techniques anciennes
- Analyse des recettes de peinture
- Compréhension des altérations

## Avantages par Rapport aux Autres Techniques

| Technique | Invasive? | Résolution | Profondeur | Information |
|-----------|-------------|-------------|-------------|-------------|
| Rayons X | Non | ~10 μm | ~1 cm | Éléments lourds |
| Spectroscopie IR | Non | ~100 μm | ~100 μm | Molécules |
| Carottage | Oui | ~1 μm | ~mm | Structure complète |
| **Électrons** | **Non** | **~100 μm** | **~500 μm** | **Éléments + structure** |

## Limitations et Solutions

### **Limitations**
- **Profondeur limitée**: ~500 μm avec 5 MeV
- **Dommages potentiels**: Si dose trop élevée
- **Complexité**: Analyse multi-paramètres

### **Solutions**
- **Énergie variable**: Adaptation selon profondeur
- **Contrôle de dose**: Monitoring en temps réel
- **Algorithmes ML**: Classification automatique

Cette technique offre un excellent compromis entre précision, non-invasivité et richesse d'information pour l'analyse d'œuvres d'art.
