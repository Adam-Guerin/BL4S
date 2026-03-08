# Geant4 Environment Setup Script
# Run this script in PowerShell to set up Geant4 environment

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$G4INSTALL = Join-Path $scriptRoot "geant4-install"
$G4BIN = "$G4INSTALL\bin"
$G4LIB = "$G4INSTALL\lib"

$dataCandidates = @(
    (Join-Path $G4INSTALL "share\Geant4\data"),
    (Join-Path $scriptRoot "geant4-build\data")
)

$G4DATA = $null
foreach ($candidate in $dataCandidates) {
    if (Test-Path $candidate) {
        $G4DATA = $candidate
        break
    }
}

# Add to PATH
if (Test-Path $G4BIN) {
    $env:PATH = "$G4BIN;$env:PATH"
}

# Set Geant4 environment variables
$env:GEANT4_INSTALL = $G4INSTALL
$env:GEANT4_BIN = $G4BIN
$env:GEANT4_LIB = $G4LIB
$env:GEANT4_DATA = $G4DATA
$env:GEANT4_DIR = "$G4LIB\cmake\Geant4"

if ($G4DATA) {
    $datasetMap = @(
        @{ Var = "G4NEUTRONHPDATA"; Prefix = "G4NDL" },
        @{ Var = "G4LEDATA"; Prefix = "G4EMLOW" },
        @{ Var = "G4LEVELGAMMADATA"; Prefix = "PhotonEvaporation" },
        @{ Var = "G4RADIOACTIVEDATA"; Prefix = "RadioactiveDecay" },
        @{ Var = "G4PARTICLEXSDATA"; Prefix = "G4PARTICLEXS" },
        @{ Var = "G4PIIDATA"; Prefix = "G4PII" },
        @{ Var = "G4REALSURFACEDATA"; Prefix = "RealSurface" },
        @{ Var = "G4SAIDXSDATA"; Prefix = "G4SAIDDATA" },
        @{ Var = "G4ABLADATA"; Prefix = "G4ABLA" },
        @{ Var = "G4INCLDATA"; Prefix = "G4INCL" },
        @{ Var = "G4ENSDFSTATEDATA"; Prefix = "G4ENSDFSTATE" },
        @{ Var = "G4CHANNELINGDATA"; Prefix = "G4CHANNELING" }
    )

    foreach ($entry in $datasetMap) {
        $datasetDir = Get-ChildItem -Path $G4DATA -Directory |
            Where-Object { $_.Name -like "$($entry.Prefix)*" } |
            Select-Object -First 1

        if ($datasetDir) {
            Set-Item -Path "Env:$($entry.Var)" -Value $datasetDir.FullName
        }
    }
}

# Display environment information
Write-Host "Geant4 environment configured:" -ForegroundColor Green
Write-Host "GEANT4_INSTALL = $env:GEANT4_INSTALL" -ForegroundColor Yellow
Write-Host "GEANT4_BIN = $env:GEANT4_BIN" -ForegroundColor Yellow
Write-Host "GEANT4_LIB = $env:GEANT4_LIB" -ForegroundColor Yellow
Write-Host "GEANT4_DATA = $env:GEANT4_DATA" -ForegroundColor Yellow
Write-Host "GEANT4_DIR = $env:GEANT4_DIR" -ForegroundColor Yellow
Write-Host ""
Write-Host "Geant4 libraries and executables are now in your PATH" -ForegroundColor Green
Write-Host "To make these changes permanent, add them to your system environment variables" -ForegroundColor Cyan
