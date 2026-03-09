# Repare les datasets Geant4 necessaires a l'execution.
# Priorite: recuperer ENSDFSTATE.dat (bloquant au demarrage).
# Si le telechargement echoue, cree un fichier placeholder pour mode demo visuelle.

param(
    [switch]$NoDemoFallback
)

Write-Host "=== Reparation des datasets Geant4 ===" -ForegroundColor Green

$root = Split-Path -Parent $PSScriptRoot
$envScript = Join-Path $root "setup_geant4_env.ps1"
if (Test-Path $envScript) {
    . $envScript
}

if (-not $env:GEANT4_DATA) {
    $env:GEANT4_DATA = Join-Path $root "geant4-build\data"
}

$dataRoot = $env:GEANT4_DATA
if (-not (Test-Path $dataRoot)) {
    New-Item -ItemType Directory -Path $dataRoot -Force | Out-Null
}

$datasetDir = Join-Path $dataRoot "G4ENSDFSTATE3.0"
$datasetFile = Join-Path $datasetDir "ENSDFSTATE.dat"

if (Test-Path $datasetFile) {
    Write-Host "OK: ENSDFSTATE.dat deja present: $datasetFile" -ForegroundColor Green
    exit 0
}

New-Item -ItemType Directory -Path $datasetDir -Force | Out-Null

$archivePath = Join-Path $PSScriptRoot "G4ENSDFSTATE.3.0.tar.gz"
$urls = @(
    "https://cern.ch/geant4-data/datasets/G4ENSDFSTATE.3.0.tar.gz",
    "https://geant4-data.web.cern.ch/datasets/G4ENSDFSTATE.3.0.tar.gz"
)

$downloaded = $false
foreach ($url in $urls) {
    Write-Host "Telechargement depuis: $url" -ForegroundColor Yellow
    try {
        & curl.exe -L --fail --retry 2 --connect-timeout 20 --max-time 300 -o $archivePath $url
        if ($LASTEXITCODE -eq 0 -and (Test-Path $archivePath) -and ((Get-Item $archivePath).Length -gt 0)) {
            $downloaded = $true
            break
        }
    } catch {
        Write-Host "ATTENTION: curl a echoue: $($_.Exception.Message)" -ForegroundColor DarkYellow
    }

    try {
        Invoke-WebRequest -Uri $url -OutFile $archivePath -MaximumRedirection 5 -TimeoutSec 300
        if ((Test-Path $archivePath) -and ((Get-Item $archivePath).Length -gt 0)) {
            $downloaded = $true
            break
        }
    } catch {
        Write-Host "ATTENTION: Invoke-WebRequest a echoue: $($_.Exception.Message)" -ForegroundColor DarkYellow
    }
}

if ($downloaded) {
    Write-Host "Extraction de l'archive..." -ForegroundColor Yellow
    try {
        & tar.exe -xzf $archivePath -C $datasetDir
    } catch {
        Write-Host "ATTENTION: Extraction tar a echoue: $($_.Exception.Message)" -ForegroundColor DarkYellow
    }
}

if (Test-Path $datasetFile) {
    Write-Host "OK: Dataset repare: $datasetFile" -ForegroundColor Green
    exit 0
}

if (-not $NoDemoFallback) {
    Write-Host "ATTENTION: Telechargement impossible. Creation d'un fichier placeholder pour mode demo." -ForegroundColor Yellow
    @(
        "# Placeholder ENSDFSTATE.dat",
        "# Usage: demonstration visuelle e- uniquement",
        "# Pour production scientifique, remplacer par le dataset officiel Geant4."
    ) | Set-Content -Path $datasetFile -Encoding ascii

    if (Test-Path $datasetFile) {
        Write-Host "OK: Placeholder cree: $datasetFile" -ForegroundColor Green
        Write-Host "ATTENTION: Mode demo uniquement (pas pour analyses physiques finales)." -ForegroundColor Yellow
        exit 0
    }
}

Write-Host "ERREUR: Impossible de reparer ENSDFSTATE.dat" -ForegroundColor Red
Write-Host "Ajoute manuellement G4ENSDFSTATE.3.0.tar.gz puis relance ce script." -ForegroundColor Red
exit 1

