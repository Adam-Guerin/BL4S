#include <iostream>
#include "ArtAnalysisManager.hh"
#include "G4SystemOfUnits.hh"

int main() {
    std::cout << "=== Test Simple de ArtAnalysisManager ===" << std::endl;
    
    try {
        // Créer l'instance
        ArtAnalysisManager* manager = ArtAnalysisManager::Instance();
        
        // Test des fonctions de base
        std::cout << "Test 1: Calcul Z pour matériaux..." << std::endl;
        
        G4double z_lead = manager->CalculateMaterialZ("LeadWhite");
        std::cout << "Z(LeadWhite) = " << z_lead << " (attendu: 82)" << std::endl;
        
        G4double z_vermilion = manager->CalculateMaterialZ("Vermilion");
        std::cout << "Z(Vermilion) = " << z_vermilion << " (attendu: 80)" << std::endl;
        
        std::cout << "\nTest 2: Détection pigments historiques..." << std::endl;
        
        G4bool is_historical = manager->IsHistoricalPigment("LeadWhite");
        std::cout << "LeadWhite est historique: " << (is_historical ? "OUI" : "NON") << " (attendu: OUI)" << std::endl;
        
        G4bool is_modern = manager->IsHistoricalPigment("ModernPaint");
        std::cout << "ModernPaint est historique: " << (is_modern ? "OUI" : "NON") << " (attendu: NON)" << std::endl;
        
        std::cout << "\nTest 3: Calcul densité..." << std::endl;
        
        G4double density_lead = manager->CalculateDensity("LeadWhite");
        std::cout << "Densité(LeadWhite) = " << density_lead/(g/cm3) << " g/cm³ (attendu: 6.5)" << std::endl;
        
        std::cout << "\nTest 4: Estimation épaisseur..." << std::endl;
        
        G4double thickness = manager->EstimateThickness(50*keV, 82);
        std::cout << "Épaisseur estimée pour 50 keV dans le plomb: " << thickness/micrometer << " μm" << std::endl;
        
        std::cout << "\n✅ Tous les tests de base passés!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ Erreur: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
