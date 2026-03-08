# Geant4 Installation Guide

## Installation Summary

Geant4 version 11.4.0 has been successfully installed on Windows with the following components:

### Installed Components
- **Geant4 Version**: 11.4.0 (Multi-threaded)
- **Build Type**: Release
- **Compiler**: MSVC 19.44.35222.0 (Visual Studio 2022)
- **CMake Version**: 4.2.3
- **Installation Path**: `C:\Users\adamg\3D Objects\5. Atelier Ecole\IQRA\geant4-install`

### Directory Structure
```
geant4-install/
├── bin/           # Executables and DLLs
├── include/       # Header files
├── lib/           # Library files
├── share/         # Data files and examples
└── lib/cmake/     # CMake configuration files
```

## Usage

### Setting Up Environment

Run the PowerShell script to set up the environment:

```powershell
.\setup_geant4_env.ps1
```

Or manually set the environment variables:

```powershell
$env:GEANT4_INSTALL = "C:\Users\adamg\3D Objects\5. Atelier Ecole\IQRA\geant4-install"
$env:PATH = "$env:GEANT4_INSTALL\bin;$env:PATH"
```

### Building Geant4 Applications

Use CMake with the following template:

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyGeant4App)

# Find Geant4
find_package(Geant4 REQUIRED)

# Add executable
add_executable(myapp main.cpp)

# Link with Geant4
target_link_libraries(myapp ${Geant4_LIBRARIES})
target_include_directories(myapp PRIVATE ${Geant4_INCLUDE_DIRS})
```

Configure and build:

```bash
mkdir build
cd build
cmake -DGeant4_DIR="C:\Users\adamg\3D Objects\5. Atelier Ecole\IQRA\geant4-install\lib\cmake\Geant4" ..
cmake --build . --config Release
```

### Data Files

Geant4 data files are located at:
```
C:\Users\adamg\3D Objects\5. Atelier Ecole\IQRA\geant4-install\share\Geant4\data\
```

The following datasets are configured but need to be downloaded separately:
- G4NDL (4.7.1)
- G4EMLOW (8.8)
- PhotonEvaporation (6.1.2)
- RadioactiveDecay (6.1.2)
- G4PARTICLEXS (4.2)
- G4PII (1.3)
- RealSurface (2.2)
- G4SAIDDATA (2.0)
- G4ABLA (3.3)
- G4INCL (1.3)
- G4ENSDFSTATE (3.0)
- G4CHANNELING (2.0)

To download datasets, reconfigure with:
```bash
cmake -DGEANT4_INSTALL_DATA=ON -DGeant4_DIR="...\geant4-install\lib\cmake\Geant4" ..
```

## Verification

The installation has been verified with a test program that successfully:
- Compiles with Geant4 headers
- Links with Geant4 libraries
- Reports the correct Geant4 version

## Next Steps

1. Download and install the required data files for physics simulations
2. Explore the example applications in `geant4-install/share/Geant4/examples/`
3. Set up your development environment with the provided scripts
4. Start building your Geant4 applications!

## Notes

- Qt GUI support was not installed (optional dependency)
- The installation uses multi-threaded build by default
- All required libraries and headers are properly configured
- The setup script provides easy environment configuration
