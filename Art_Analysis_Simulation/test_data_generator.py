#!/usr/bin/env python3
"""
Générateur de données de test pour la simulation d'analyse d'art
Crée des fichiers CSV avec des résultats réalistes pour tester les algorithmes
"""

import numpy as np
import pandas as pd
import random
from datetime import datetime

class ArtTestDataGenerator:
    def __init__(self):
        self.materials = {
            'Varnish': {'Z': 6, 'density': 1.2, 'color': '#FFEB99'},
            'ModernRestoration': {'Z': 22, 'density': 2.0, 'color': '#8B4513'},
            'Vermilion': {'Z': 80, 'density': 8.1, 'color': '#DC143C'},
            'LapisLazuli': {'Z': 14, 'density': 2.4, 'color': '#4169E1'},
            'LeadWhite': {'Z': 82, 'density': 6.5, 'color': '#D3D3D3'},
            'Malachite': {'Z': 29, 'density': 4.0, 'color': '#228B22'},
            'LeadTinYellow': {'Z': 50, 'density': 6.8, 'color': '#FFD700'},
            'Gesso': {'Z': 20, 'density': 1.8, 'color': '#F5DEB3'},
            'Canvas': {'Z': 6, 'density': 1.5, 'color': '#DEB887'}
        }
        
        self.thicknesses = {
            'Varnish': 30,
            'ModernRestoration': 80,
            'Vermilion': 60,
            'LapisLazuli': 150,
            'LeadWhite': 200,
            'Malachite': 40,
            'LeadTinYellow': 50,
            'Gesso': 500,
            'Canvas': 1000
        }
    
    def calculate_energy_loss(self, material, thickness, num_electrons=1000):
        """Calcule la perte d'énergie réaliste basée sur Z et l'épaisseur"""
        props = self.materials[material]
        Z = props['Z']
        density = props['density']
        
        # Formule simplifiée: dE/dx ∝ Z * densité * sqrt(épaisseur)
        base_loss = 0.1 * Z * np.sqrt(density) * np.sqrt(thickness/100)
        
        # Ajouter du bruit gaussien
        noise = np.random.normal(0, base_loss * 0.1, num_electrons)
        energy_losses = base_loss + noise
        
        return np.clip(energy_losses, 0.1, 100)  # Limiter entre 0.1 et 100 keV
    
    def calculate_scattering_angle(self, material, thickness, num_electrons=1000):
        """Calcule l'angle de diffusion réaliste"""
        props = self.materials[material]
        Z = props['Z']
        
        # Diffusion multiple: θ ∝ Z * sqrt(épaisseur)
        base_angle = 0.01 * Z * np.sqrt(thickness/100)
        
        # Ajouter du bruit
        noise = np.random.normal(0, base_angle * 0.2, num_electrons)
        angles = base_angle + noise
        
        return np.clip(angles, 0.1, 50)  # Limiter entre 0.1 et 50 mrad
    
    def generate_single_analysis(self, num_electrons=10000):
        """Génère une analyse complète pour une œuvre"""
        data = []
        
        for material in self.materials.keys():
            thickness = self.thicknesses[material]
            
            # Calculer les propriétés pour ce matériau
            energy_losses = self.calculate_energy_loss(material, thickness, num_electrons)
            scattering_angles = self.calculate_scattering_angle(material, thickness, num_electrons)
            
            # Statistiques
            mean_energy = np.mean(energy_losses)
            std_energy = np.std(energy_losses)
            mean_angle = np.mean(scattering_angles)
            std_angle = np.std(scattering_angles)
            
            # Compter les électrons qui ont traversé (simulation)
            detected_electrons = int(num_electrons * (0.7 + 0.3 * np.random.random()))
            
            data.append({
                'Matériau': material,
                'Épaisseur_réelle(μm)': thickness,
                'Perte_Énergie_moyenne(keV)': mean_energy,
                'Perte_Énergie_std(keV)': std_energy,
                'Angle_Diffusion_moyen(mrad)': mean_angle,
                'Angle_Diffusion_std(mrad)': std_angle,
                'Électrons_détectés': detected_electrons,
                'Numéro_Atomique': self.materials[material]['Z'],
                'Densité(g/cm³)': self.materials[material]['density'],
                'Couleur': self.materials[material]['color']
            })
        
        return pd.DataFrame(data)
    
    def generate_comparison_study(self):
        """Génère une étude comparative: œuvre ancienne vs moderne"""
        
        # Œuvre ancienne (tous les pigments historiques)
        ancient_art = self.generate_single_analysis(8000)
        ancient_art['Type'] = 'Ancienne'
        ancient_art['Époque'] = 'Avant 1900'
        
        # Œuvre moderne (seulement pigments modernes)
        modern_data = []
        modern_materials = ['Varnish', 'ModernRestoration', 'Gesso', 'Canvas']
        
        for material in modern_materials:
            thickness = self.thicknesses[material] * 1.2  # Plus épais en général
            
            energy_losses = self.calculate_energy_loss(material, thickness, 8000)
            scattering_angles = self.calculate_scattering_angle(material, thickness, 8000)
            
            modern_data.append({
                'Matériau': material,
                'Épaisseur_réelle(μm)': thickness,
                'Perte_Énergie_moyenne(keV)': np.mean(energy_losses),
                'Angle_Diffusion_moyen(mrad)': np.mean(scattering_angles),
                'Électrons_détectés': int(8000 * 0.8),
                'Numéro_Atomique': self.materials[material]['Z'],
                'Type': 'Moderne',
                'Époque': 'XXème-XXIème siècle'
            })
        
        modern_art = pd.DataFrame(modern_data)
        
        # Combiner les deux
        comparison = pd.concat([ancient_art, modern_art], ignore_index=True)
        return comparison
    
    def generate_time_series(self, days=30):
        """Génère des données de monitoring dans le temps"""
        dates = pd.date_range(start=datetime.now(), periods=days, freq='D')
        
        data = []
        base_degradation = 0.001  # 0.1% par jour
        
        for date in dates:
            # Simuler la dégradation lente des pigments
            day_factor = (date - dates[0]).days
            
            for material in ['LeadWhite', 'Vermilion', 'LapisLazuli']:
                # Les pigments historiques se dégradent différemment
                if material == 'Vermilion':
                    degradation = base_degradation * 1.5  # Plus rapide
                elif material == 'LapisLazuli':
                    degradation = base_degradation * 0.8  # Plus lent
                else:
                    degradation = base_degradation
                
                thickness = self.thicknesses[material] * (1 - degradation * day_factor)
                energy_loss = np.mean(self.calculate_energy_loss(material, thickness, 1000))
                
                data.append({
                    'Date': date,
                    'Matériau': material,
                    'Épaisseur(μm)': thickness,
                    'Perte_Énergie(keV)': energy_loss,
                    'Dégradation(%)': degradation * day_factor * 100
                })
        
        return pd.DataFrame(data)
    
    def save_all_test_data(self):
        """Génère et sauvegarde tous les jeux de données de test"""
        
        print("🎨 Génération des données de test pour la simulation d'art")
        print("=" * 60)
        
        # 1. Analyse standard
        print("📊 Génération de l'analyse standard...")
        standard_analysis = self.generate_single_analysis(10000)
        standard_analysis.to_csv('test_standard_analysis.csv', index=False)
        print("✓ test_standard_analysis.csv créé")
        
        # 2. Étude comparative
        print("🔬 Génération de l'étude comparative...")
        comparison = self.generate_comparison_study()
        comparison.to_csv('test_comparison_study.csv', index=False)
        print("✓ test_comparison_study.csv créé")
        
        # 3. Série temporelle
        print("📈 Génération de la série temporelle...")
        time_series = self.generate_time_series(30)
        time_series.to_csv('test_time_series.csv', index=False)
        print("✓ test_time_series.csv créé")
        
        # 4. Données de validation (avec épaisseurs connues)
        print("🔍 Génération des données de validation...")
        validation_data = []
        
        for material in self.materials.keys():
            for thickness in [20, 50, 100, 200, 500]:
                energy_loss = np.mean(self.calculate_energy_loss(material, thickness, 5000))
                scattering = np.mean(self.calculate_scattering_angle(material, thickness, 5000))
                
                validation_data.append({
                    'Matériau': material,
                    'Épaisseur_vraie(μm)': thickness,
                    'Perte_Énergie_mesurée(keV)': energy_loss,
                    'Angle_Diffusion_mesuré(mrad)': scattering,
                    'Z': self.materials[material]['Z'],
                    'Densité': self.materials[material]['density']
                })
        
        validation_df = pd.DataFrame(validation_data)
        validation_df.to_csv('test_validation_data.csv', index=False)
        print("✓ test_validation_data.csv créé")
        
        # 5. Résumé statistique
        print("📋 Génération du résumé statistique...")
        summary = {
            'Fichier': ['test_standard_analysis.csv', 'test_comparison_study.csv', 
                       'test_time_series.csv', 'test_validation_data.csv'],
            'Lignes': [len(standard_analysis), len(comparison), 
                      len(time_series), len(validation_df)],
            'Description': ['Analyse complète d\'une œuvre', 
                          'Comparaison ancien vs moderne',
                          'Monitoring temporel (30 jours)',
                          'Données de validation']
        }
        
        summary_df = pd.DataFrame(summary)
        summary_df.to_csv('test_data_summary.csv', index=False)
        print("✓ test_data_summary.csv créé")
        
        print("\n🎉 Tous les fichiers de test ont été générés!")
        print("\nUtilisation:")
        print("- test_standard_analysis.csv: Pour tester l'analyse complète")
        print("- test_comparison_study.csv: Pour comparer les époques")
        print("- test_time_series.csv: Pour tester le monitoring")
        print("- test_validation_data.csv: Pour valider les algorithmes")
        print("- test_data_summary.csv: Résumé de tous les fichiers")

if __name__ == "__main__":
    generator = ArtTestDataGenerator()
    generator.save_all_test_data()
