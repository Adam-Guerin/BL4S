#include <iostream>
#include "G4Version.hh"

int main() {
    std::cout << "=== Geant4 Installation Test ===" << std::endl;
    std::cout << "Geant4 Version: " << G4Version << std::endl;
    std::cout << "Geant4 is successfully installed!" << std::endl;
    return 0;
}
