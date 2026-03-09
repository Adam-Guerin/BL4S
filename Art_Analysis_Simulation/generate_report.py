#!/usr/bin/env python3
"""
Script Python pour générer des visualisations et rapports d'analyse d'œuvres d'art
À utiliser avec les données de la simulation Geant4
"""

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.patches import Rectangle
import seaborn as sns
from datetime import datetime

def load_analysis_data(filename="analysis_results.csv"):
    """Charge les données d'analyse depuis le fichier CSV"""
    try:
        df = pd.read_csv(filename)
        return df
    except FileNotFoundError:
        print(f"Fichier {filename} non trouvé. Utilisation de données de démonstration.")
        # Données de démonstration basées sur les pigments historiques
        return pd.DataFrame({
            'Matériau': ['Varnish', 'ModernRestoration', 'Vermilion', 'LapisLazuli', 
                        'LeadWhite', 'Malachite', 'LeadTinYellow', 'Gesso', 'Canvas'],
            'Épaisseur(μm)': [30, 80, 60, 150, 200, 40, 50, 500, 1000],
            'Perte_Énergie(keV)': [5, 12, 25, 18, 35, 15, 20, 8, 3],
            'Angle_Diffusion(mrad)': [2, 5, 15, 8, 20, 12, 18, 4, 1],
            'Nb_Électrons': [4500, 4200, 3800, 4000, 3500, 3900, 3700, 4400, 4600]
        })

def create_layer_visualization(df):
    """Crée une visualisation des couches de l'œuvre"""
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 8))
    
    # Couleurs pour chaque matériau
    colors = {
        'Varnish': '#FFEB99',
        'ModernRestoration': '#8B4513',
        'Vermilion': '#DC143C',
        'LapisLazuli': '#4169E1',
        'LeadWhite': '#D3D3D3',
        'Malachite': '#228B22',
        'LeadTinYellow': '#FFD700',
        'Gesso': '#F5DEB3',
        'Canvas': '#DEB887'
    }
    
    # Visualisation 1: Coupe transversale de l'œuvre
    y_pos = 0
    for _, row in df.iterrows():
        material = row['Matériau']
        thickness = row['Épaisseur(μm)']
        color = colors.get(material, '#808080')
        
        rect = Rectangle((0, y_pos), 10, thickness, 
                        facecolor=color, edgecolor='black', linewidth=1)
        ax1.add_patch(rect)
        
        # Ajouter le nom du matériau
        ax1.text(5, y_pos + thickness/2, material, 
                ha='center', va='center', fontsize=8, fontweight='bold')
        
        y_pos += thickness
    
    ax1.set_xlim(0, 10)
    ax1.set_ylim(0, df['Épaisseur(μm)'].sum())
    ax1.set_xlabel('Largeur (unités arbitraires)')
    ax1.set_ylabel('Épaisseur (μm)')
    ax1.set_title('Structure en couches de l\'œuvre d\'art')
    ax1.grid(True, alpha=0.3)
    
    # Visualisation 2: Perte d'énergie par couche
    bars = ax2.bar(df['Matériau'], df['Perte_Énergie(keV)'], 
                   color=[colors.get(mat, '#808080') for mat in df['Matériau']])
    ax2.set_xlabel('Matériau')
    ax2.set_ylabel('Perte d\'énergie moyenne (keV)')
    ax2.set_title('Perte d\'énergie des électrons par couche')
    ax2.tick_params(axis='x', rotation=45)
    ax2.grid(True, alpha=0.3)
    
    # Ajouter les valeurs sur les barres
    for bar, value in zip(bars, df['Perte_Énergie(keV)']):
        height = bar.get_height()
        ax2.text(bar.get_x() + bar.get_width()/2., height,
                f'{value:.1f}', ha='center', va='bottom')
    
    plt.tight_layout()
    plt.savefig('artwork_layers.png', dpi=300, bbox_inches='tight')
    plt.show()

def create_scattering_analysis(df):
    """Crée une analyse de la diffusion des électrons"""
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    
    # 1. Angle de diffusion vs épaisseur
    scatter = ax1.scatter(df['Épaisseur(μm)'], df['Angle_Diffusion(mrad)'], 
                          s=df['Perte_Énergie(keV)']*10, alpha=0.6,
                          c=df['Perte_Énergie(keV)'], cmap='viridis')
    ax1.set_xlabel('Épaisseur (μm)')
    ax1.set_ylabel('Angle de diffusion (mrad)')
    ax1.set_title('Relation épaisseur-diffusion')
    plt.colorbar(scatter, ax=ax1, label='Perte d\'énergie (keV)')
    
    # Ajouter les noms des matériaux
    for _, row in df.iterrows():
        ax1.annotate(row['Matériau'][:8], 
                    (row['Épaisseur(μm)'], row['Angle_Diffusion(mrad)']),
                    fontsize=8, alpha=0.7)
    
    # 2. Perte d'énergie vs numéro atomique effectif
    z_values = {'Varnish': 6, 'ModernRestoration': 22, 'Vermilion': 80, 
                'LapisLazuli': 14, 'LeadWhite': 82, 'Malachite': 29, 
                'LeadTinYellow': 50, 'Gesso': 20, 'Canvas': 6}
    
    df['Z_effectif'] = df['Matériau'].map(z_values)
    ax2.scatter(df['Z_effectif'], df['Perte_Énergie(keV)'], 
               s=100, alpha=0.7, c='red')
    ax2.set_xlabel('Numéro atomique effectif')
    ax2.set_ylabel('Perte d\'énergie (keV)')
    ax2.set_title('Perte d\'énergie vs Z effectif')
    ax2.grid(True, alpha=0.3)
    
    # 3. Histogramme des angles de diffusion
    ax3.hist(df['Angle_Diffusion(mrad)'], bins=10, alpha=0.7, color='skyblue', edgecolor='black')
    ax3.set_xlabel('Angle de diffusion (mrad)')
    ax3.set_ylabel('Fréquence')
    ax3.set_title('Distribution des angles de diffusion')
    ax3.grid(True, alpha=0.3)
    
    # 4. Camembert des types de matériaux
    historical = ['Vermilion', 'LapisLazuli', 'LeadWhite', 'Malachite', 'LeadTinYellow']
    modern = ['ModernRestoration']
    support = ['Varnish', 'Gesso', 'Canvas']
    
    hist_count = sum(df['Matériau'].isin(historical))
    mod_count = sum(df['Matériau'].isin(modern))
    sup_count = sum(df['Matériau'].isin(support))
    
    sizes = [hist_count, mod_count, sup_count]
    labels = ['Pigments historiques', 'Restauration moderne', 'Support/Préparation']
    colors = ['#FF6B6B', '#4ECDC4', '#45B7D1']
    
    ax4.pie(sizes, labels=labels, colors=colors, autopct='%1.1f%%', startangle=90)
    ax4.set_title('Composition des matériaux de l\'œuvre')
    
    plt.tight_layout()
    plt.savefig('scattering_analysis.png', dpi=300, bbox_inches='tight')
    plt.show()

def generate_detailed_report(df):
    """Génère un rapport détaillé de l'analyse"""
    report = f"""
# RAPPORT D'ANALYSE D'ŒUVRE D'ART PAR FAISCEAU D'ÉLECTRONS
Généré le: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

## RÉSUMÉ DE L'ŒUVRE
- Nombre total de couches détectées: {len(df)}
- Épaisseur totale: {df['Épaisseur(μm)'].sum():.1f} μm
- Perte d'énergie totale: {df['Perte_Énergie(keV)'].sum():.1f} keV
- Angle de diffusion moyen: {df['Angle_Diffusion(mrad)'].mean():.2f} mrad

## ANALYSE DES COUCHES
"""
    
    for _, row in df.iterrows():
        material = row['Matériau']
        thickness = row['Épaisseur(μm)']
        energy_loss = row['Perte_Énergie(keV)']
        angle = row['Angle_Diffusion(mrad)']
        
        # Classification du matériau
        if material in ['Vermilion', 'LapisLazuli', 'LeadWhite', 'Malachite', 'LeadTinYellow']:
            category = "🎨 PIGMENT HISTORIQUE"
            era = "Avant 1900"
        elif material == 'ModernRestoration':
            category = "🔧 RESTAURATION MODERNE"
            era = "XXème-XXIème siècle"
        else:
            category = "📄 SUPPORT/PRÉPARATION"
            era = "Varie"
        
        report += f"""
### {material}
- **Catégorie**: {category}
- **Époque typique**: {era}
- **Épaisseur**: {thickness:.1f} μm
- **Perte d'énergie**: {energy_loss:.2f} keV
- **Angle de diffusion**: {angle:.2f} mrad
- **Électrons détectés**: {row['Nb_Électrons']}
"""
        
        # Analyse spécifique
        if energy_loss > 20:
            report += "- **Analyse**: Matériau dense provoquant une forte perte d'énergie\n"
        if angle > 10:
            report += "- **Analyse**: Diffusion multiple élevée caractéristique\n"
    
    report += f"""
## CONCLUSIONS

### PIGMENTS HISTORIQUES DÉTECTÉS
"""
    
    historical_materials = df[df['Matériau'].isin(['Vermilion', 'LapisLazuli', 'LeadWhite', 'Malachite', 'LeadTinYellow'])]
    if not historical_materials.empty:
        report += "✓ **Présence confirmée de pigments historiques authentiques**\n"
        for _, row in historical_materials.iterrows():
            report += f"- {row['Matériau']}: {row['Épaisseur(μm)']:.1f} μm\n"
    else:
        report += "✗ Aucun pigment historique détecté\n"
    
    report += f"""
### RESTAURATIONS
"""
    if 'ModernRestoration' in df['Matériau'].values:
        report += "✓ **Restauration moderne détectée** - Présence de peinture contemporaine\n"
    else:
        report += "✓ Aucune restauration moderne détectée\n"
    
    report += f"""
### AUTHENTICITÉ
"""
    historical_count = len(historical_materials)
    if historical_count >= 3:
        report += "🏆 **Œuvre de grand intérêt historique** - Multiples pigments authentiques\n"
    elif historical_count >= 1:
        report += "⭐ **Œuvre d'intérêt historique** - Pigments historiques présents\n"
    else:
        report += "📋 **Œuvre contemporaine** - Pas de pigments historiques détectés\n"
    
    # Sauvegarder le rapport
    with open('artwork_analysis_report.md', 'w', encoding='utf-8') as f:
        f.write(report)
    
    print("Rapport détaillé sauvegardé dans 'artwork_analysis_report.md'")
    return report

def main():
    """Fonction principale"""
    print("🎨 Génération du rapport d'analyse d'œuvre d'art")
    print("=" * 50)
    
    # Charger les données
    df = load_analysis_data()
    
    # Créer les visualisations
    print("📊 Création des visualisations...")
    create_layer_visualization(df)
    create_scattering_analysis(df)
    
    # Générer le rapport
    print("📝 Génération du rapport détaillé...")
    report = generate_detailed_report(df)
    
    print("\n✅ Analyse terminée!")
    print("Fichiers générés:")
    print("- artwork_layers.png: Visualisation des couches")
    print("- scattering_analysis.png: Analyse de la diffusion")
    print("- artwork_analysis_report.md: Rapport détaillé")

if __name__ == "__main__":
    main()
