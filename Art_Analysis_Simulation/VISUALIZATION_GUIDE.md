# 🎨 Guide Complet de Visualisation de la Simulation

## 🎯 **Oui! La simulation a plusieurs visualisations impressionnantes!**

---

## 🖥️ **1. Visualisation 3D Interactives**

### **Visualisation Avancée** ⭐
Fichier: `advanced_visualization.mac`

**Caractéristiques:**
- 🎨 **Couleurs réalistes** pour chaque pigment
- 🔍 **Transparence** pour voir les couches internes  
- 📊 **Trajectoires d'électrons** colorées par énergie
- 🎯 **Détecteurs en surbrillance**
- ✂️ **Vue en coupe** pour voir l'intérieur

**Lancement:**
```bash
# Si compilation réussie
.\build\Release\ArtAnalysisSimulation.exe advanced_visualization.mac

# Ou mode interactif
.\build\Release\ArtAnalysisSimulation.exe
# Puis dans la console Geant4:
/control/execute advanced_visualization.mac
```

**Commandes interactives:**
- `/vis/viewer/zoom` - Zoom avant/arrière
- `/vis/viewer/pan` - Déplacer la vue
- `/vis/viewer/rotate` - Rotation
- `/vis/viewer/set/cutaway false` - Désactiver coupe

---

## 🌐 **2. Interface Web Interactive**

Fichier: `user_interface.html`

**Caractéristiques:**
- 🎛️ **Interface moderne** avec sliders et boutons
- 📊 **Graphiques en temps réel** de la simulation
- 🎨 **Visualisation des couches** avec couleurs
- 📈 **Progression** avec barre de chargement
- 💾 **Export des résultats** en un clic

**Lancement:**
```bash
# Ouvre dans votre navigateur par défaut
start user_interface.html
```

---

## 📊 **3. Visualisation des Données Python**

Fichier: `generate_report.py`

**Caractéristiques:**
- 📈 **Graphiques matplotlib** des résultats
- 🎨 **Visualisation des couches** en 2D/3D
- 📊 **Histogrammes** de distribution d'énergie
- 🗺️ **Cartographie** des pigments détectés
- 📄 **Rapports automatiques** en Markdown

**Lancement:**
```bash
python generate_report.py
```

---

## 🔬 **4. Visualisation Scientifique**

### **Trajectoires d'Électrons**
- 🎯 **Couleurs par charge** (électrons en bleu)
- 📏 **Points d'étape** pour voir les interactions
- 🌈️ **Gradient par énergie** (rouge=haute, bleu=basse)

### **Couleurs des Matériaux**
| Matériau | Couleur RGB | Transparence |
|-----------|--------------|--------------|
| Vernis | (1.0, 1.0, 0.8) | 30% |
| Restauration | (0.6, 0.3, 0.1) | 50% |
| Vermillon | (0.9, 0.1, 0.1) | 70% |
| Lapis-lazuli | (0.1, 0.2, 0.8) | 60% |
| Blanc de plomb | (0.8, 0.8, 0.8) | 40% |
| Malachite | (0.1, 0.6, 0.3) | 60% |
| Jaune plomb-étain | (0.9, 0.7, 0.2) | 70% |
| Gesso | (0.9, 0.9, 0.7) | 30% |
| Toile | (0.8, 0.7, 0.5) | 50% |

### **Détecteurs**
- 🟢 **Trackers** en vert brillant
- 🔴 **Calorimètres** en rouge vif
- 📏 **Dimensions optimisées** pour précision

---

## 🎮 **5. Commandes de Visualisation**

### **Configuration de Base**
```bash
/vis/open OGL 1200x800-0+0     # Fenêtre 1200x800
/vis/drawVolume                   # Dessiner tous les volumes
/vis/viewer/set/style surface      # Style solide
/vis/viewer/set/auxiliaryEdge true # Bords apparents
```

### **Trajectoires**
```bash
/vis/trajectory/default/drawByCharge    # Couleur par charge
/vis/trajectory/default/drawByEnergy     # Couleur par énergie
/vis/trajectory/default/drawStepPts true # Points d'étape
/vis/trajectory/default/stepPtsSize 3   # Taille des points
```

### **Vue et Caméra**
```bash
/vis/viewer/set/viewpointVector -1 0.5 0.3  # Position caméra
/vis/viewer/set/lightsVector -1 0.5 0.3      # Position lumières
/vis/viewer/set/cutaway true                    # Activer coupe
/vis/viewer/set/cutawayPlane 0 0 1 0          # Plan de coupe
```

---

## 🚀 **6. Lancement Rapide**

### **Option 1: Visualisation 3D Complète**
```bash
# Script de lancement
start_visualization_3d.ps1
```

### **Option 2: Interface Web**
```bash
# Interface utilisateur
start user_interface.html
```

### **Option 3: Mode Interactif**
```bash
# Mode console Geant4
.\build\Release\ArtAnalysisSimulation.exe
# Puis charger les macros:
/control/execute advanced_visualization.mac
```

---

## 📱 **7. Personnalisation**

### **Modifier les Couleurs**
Dans `advanced_visualization.mac`:
```bash
# Changer la couleur du vernis
/vis/geometry/set/colour Varnish 1.0 0.9 0.7 0.3

# Changer la couleur des trajectoires
/vis/trajectory/default/colour 0.2 0.8 1.0  # Bleu clair
```

### **Ajuster la Transparence**
```bash
# Plus transparent
/vis/geometry/set/colour LeadWhite 0.8 0.8 0.8 0.2

# Moins transparent  
/vis/geometry/set/colour Vermilion 0.9 0.1 0.1 0.9
```

### **Changer la Vue**
```bash
# Vue de dessus
/vis/viewer/set/viewpointVector 0 0 1

# Vue de côté
/vis/viewer/set/viewpointVector 1 0 0

# Vue en perspective
/vis/viewer/set/viewpointVector -1 1 0.5
```

---

## 🎯 **8. Cas d'Usage Réels**

### **Analyse d'Œuvre d'Art**
1. **Charger l'œuvre**: Configuration dans `ArtDetectorConstruction.cc`
2. **Lancer la visualisation**: `advanced_visualization.mac`
3. **Observer les interactions**: Trajectoires et dépôts d'énergie
4. **Identifier les pigments**: Couleurs et densités
5. **Mesurer les épaisseurs**: Couches visibles en coupe

### **Détection de Faux**
1. **Pigments anachroniques**: Couleurs/modèles incohérents
2. **Restaurations visibles**: Couches modernes sur anciennes
3. **Structure anormale**: Épaisseurs ou séquences inhabituelles

### **Recherche Scientifique**
1. **Optimiser les détecteurs**: Positions et tailles
2. **Tester différentes énergies**: 50 keV - 200 keV
3. **Analyser les signatures**: Patterns de diffusion
4. **Valider les modèles**: Comparaison avec œuvres authentiques

---

## 🎨 **9. Résultats Visuels Attendus**

### **Vue 3D Complète**
- 🖼️ **Structure en couches** avec couleurs distinctes
- ⚡ **Trajectoires d'électrons** traversant l'œuvre
- 📊 **Détecteurs** montrant les mesures
- 🔍 **Vue en coupe** pour voir l'intérieur

### **Interface Web**
- 📈 **Graphiques temps réel** de l'énergie perdue
- 🎨 **Visualisation interactive** des couches
- 📊 **Histogrammes** de distribution des angles
- 💾 **Export automatique** des résultats

### **Rapports Python**
- 📊 **Graphiques matplotlib** haute résolution
- 🗺️ **Cartographie 2D** des pigments
- 📈 **Analyse statistique** complète
- 📄 **Rapport Markdown** prêt à publier

---

## 🚀 **Conclusion**

**OUI! La simulation a des visualisations exceptionnelles:**

✅ **Visualisation 3D interactive** avec Geant4 OpenGL  
✅ **Interface web moderne** avec graphiques temps réel  
✅ **Rapports Python** avec matplotlib/seaborn  
✅ **Personnalisation complète** des couleurs et vues  
✅ **Analyse scientifique** avec trajectoires détaillées  

**C'est un outil professionnel d'analyse d'art!** 🎨✨
