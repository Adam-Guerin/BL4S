# Art Analysis Simulation - Automated Test Runner
# This script runs all test suites and generates comprehensive reports

param(
    [switch]$Quick,
    [switch]$Performance,
    [switch]$Physics,
    [switch]$Integration,
    [switch]$Regression,
    [switch]$Unit,
    [switch]$CreateBaseline,
    [string]$OutputDir = "test_results",
    [string]$BuildType = "Release"
)

# Initialize
$ErrorActionPreference = "Stop"
$StartTime = Get-Date

# Create output directory
if (!(Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
}

# Logging function
function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $Timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $LogEntry = "[$Timestamp] [$Level] $Message"
    Write-Host $LogEntry
    Add-Content -Path "$OutputDir\test_log.txt" -Value $LogEntry
}

# Build function
function Build-Test {
    param([string]$TestName, [string]$SourceFile)
    
    Write-Log "Building $TestName..."
    
    $BuildDir = "build_$TestName"
    if (!(Test-Path $BuildDir)) {
        New-Item -ItemType Directory -Path $BuildDir | Out-Null
    }
    
    Push-Location $BuildDir
    
    try {
        # Configure CMake
        cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=$BuildType
        if ($LASTEXITCODE -ne 0) {
            throw "CMake configuration failed for $TestName"
        }
        
        # Build
        cmake --build . --config $BuildType --target $TestName
        if ($LASTEXITCODE -ne 0) {
            throw "Build failed for $TestName"
        }
        
        Write-Log "$TestName built successfully"
        return "$BuildDir\$BuildType\$TestName.exe"
    }
    catch {
        Write-Log "Build failed for $TestName`: $_" "ERROR"
        Pop-Location
        return $null
    }
    finally {
        Pop-Location
    }
}

# Run test function
function Run-Test {
    param(
        [string]$TestName,
        [string]$ExecutablePath,
        [string]$Arguments = "",
        [string]$OutputFile = ""
    )
    
    if (!(Test-Path $ExecutablePath)) {
        Write-Log "Executable not found: $ExecutablePath" "ERROR"
        return $false
    }
    
    Write-Log "Running $TestName..."
    
    try {
        if ($OutputFile) {
            $Process = Start-Process -FilePath $ExecutablePath -ArgumentList $Arguments -RedirectStandardOutput "$OutputDir\$OutputFile" -RedirectStandardError "$OutputDir\${TestName}_error.txt" -Wait -PassThru
        } else {
            $Process = Start-Process -FilePath $ExecutablePath -ArgumentList $Arguments -RedirectStandardOutput "$OutputDir\${TestName}_output.txt" -RedirectStandardError "$OutputDir\${TestName}_error.txt" -Wait -PassThru
        }
        
        if ($Process.ExitCode -eq 0) {
            Write-Log "$TestName passed"
            return $true
        } else {
            Write-Log "$TestName failed with exit code $($Process.ExitCode)" "ERROR"
            return $false
        }
    }
    catch {
        Write-Log "Failed to run $TestName`: $_" "ERROR"
        return $false
    }
}

# Test suite definitions
$TestSuites = @{
    "unit" = @{
        "Name" = "Unit Tests"
        "Source" = "unit_tests.cc"
        "Executable" = "unit_tests"
        "Description" = "Basic functionality tests"
    }
    "integration" = @{
        "Name" = "Integration Tests"
        "Source" = "integration_tests.cc"
        "Executable" = "integration_tests"
        "Description" = "End-to-end simulation tests"
    }
    "performance" = @{
        "Name" = "Performance Tests"
        "Source" = "performance_tests.cc"
        "Executable" = "performance_tests"
        "Description" = "Speed and memory benchmarks"
    }
    "physics" = @{
        "Name" = "Physics Validation"
        "Source" = "physics_validation_tests.cc"
        "Executable" = "physics_validation_tests"
        "Description" = "Physics model validation"
    }
    "regression" = @{
        "Name" = "Regression Tests"
        "Source" = "regression_tests.cc"
        "Executable" = "regression_tests"
        "Description" = "Algorithm regression testing"
    }
}

# Main execution
Write-Log "Starting Art Analysis Simulation Test Suite"
Write-Log "Build Type: $BuildType"
Write-Log "Output Directory: $OutputDir"

$TestsToRun = @()
$Results = @{}

# Determine which tests to run
if ($Quick) {
    $TestsToRun = @("unit")
    Write-Log "Quick test mode: running unit tests only"
} elseif ($Performance) {
    $TestsToRun = @("performance")
    Write-Log "Performance test mode only"
} elseif ($Physics) {
    $TestsToRun = @("physics")
    Write-Log "Physics validation mode only"
} elseif ($Integration) {
    $TestsToRun = @("integration")
    Write-Log "Integration test mode only"
} elseif ($Regression) {
    $TestsToRun = @("regression")
    Write-Log "Regression test mode only"
} elseif ($Unit) {
    $TestsToRun = @("unit")
    Write-Log "Unit test mode only"
} else {
    $TestsToRun = $TestSuites.Keys
    Write-Log "Full test suite: running all tests"
}

# Run selected tests
foreach ($TestKey in $TestsToRun) {
    $TestSuite = $TestSuites[$TestKey]
    Write-Log "`n=== $($TestSuite.Name) ==="
    Write-Log "Description: $($TestSuite.Description)"
    
    # Build test
    $ExecutablePath = Build-Test -TestName $TestSuite["Executable"] -SourceFile $TestSuite["Source"]
    
    if ($ExecutablePath) {
        # Handle regression test special case
        $Arguments = ""
        if ($TestKey -eq "regression" -and $CreateBaseline) {
            $Arguments = "--create-baseline"
        }
        
        # Run test
        $OutputFile = "${TestKey}_results.txt"
        $Success = Run-Test -TestName $TestSuite["Name"] -ExecutablePath $ExecutablePath -Arguments $Arguments -OutputFile $OutputFile
        
        $Results[$TestKey] = @{
            "Success" = $Success
            "Suite" = $TestSuite["Name"]
            "Executable" = $ExecutablePath
        }
    } else {
        $Results[$TestKey] = @{
            "Success" = $false
            "Suite" = $TestSuite["Name"]
            "Executable" = $null
        }
    }
}

# Generate summary report
$EndTime = Get-Date
$Duration = $EndTime - $StartTime

Write-Log "`n=== Test Summary ==="
$SummaryFile = "$OutputDir\test_summary.txt"
"Art Analysis Simulation - Test Summary" | Out-File -FilePath $SummaryFile
"Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" | Out-File -FilePath $SummaryFile -Append
"Duration: $($Duration.TotalMinutes.ToString('F2')) minutes" | Out-File -FilePath $SummaryFile -Append
"Build Type: $BuildType" | Out-File -FilePath $SummaryFile -Append
"" | Out-File -FilePath $SummaryFile -Append

$PassedCount = 0
$FailedCount = 0

foreach ($Result in $Results.GetEnumerator()) {
    $Status = if ($Result.Value.Success) { "PASS" } else { "FAIL" }
    $Line = "$($Result.ValueSuite.PadRight(25)) : $Status"
    $Line | Out-File -FilePath $SummaryFile -Append
    Write-Log $Line
    
    if ($Result.Value.Success) {
        $PassedCount++
    } else {
        $FailedCount++
    }
}

"" | Out-File -FilePath $SummaryFile -Append
"Total Tests: $($Results.Count)" | Out-File -FilePath $SummaryFile -Append
"Passed: $PassedCount" | Out-File -FilePath $SummaryFile -Append
"Failed: $FailedCount" | Out-File -FilePath $SummaryFile -Append
"Success Rate: $([math]::Round($PassedCount / $Results.Count * 100, 1))%" | Out-File -FilePath $SummaryFile -Append

# Generate HTML report
$HtmlReport = @"
<!DOCTYPE html>
<html>
<head>
    <title>Art Analysis Simulation - Test Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 10px; border-radius: 5px; }
        .pass { color: green; font-weight: bold; }
        .fail { color: red; font-weight: bold; }
        .summary { background-color: #f9f9f9; padding: 10px; margin: 10px 0; border-radius: 5px; }
        table { border-collapse: collapse; width: 100%; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #f2f2f2; }
    </style>
</head>
<body>
    <div class="header">
        <h1>Art Analysis Simulation - Test Report</h1>
        <p>Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')</p>
        <p>Duration: $($Duration.TotalMinutes.ToString('F2')) minutes</p>
        <p>Build Type: $BuildType</p>
    </div>
    
    <div class="summary">
        <h2>Summary</h2>
        <p>Total Tests: $($Results.Count)</p>
        <p class="pass">Passed: $PassedCount</p>
        <p class="fail">Failed: $FailedCount</p>
        <p>Success Rate: $([math]::Round($PassedCount / $Results.Count * 100, 1))%</p>
    </div>
    
    <h2>Test Results</h2>
    <table>
        <tr><th>Test Suite</th><th>Status</th><th>Description</th></tr>
"@

foreach ($Result in $Results.GetEnumerator()) {
    $StatusClass = if ($Result.Value.Success) { "pass" } else { "fail" }
    $Status = if ($Result.Value.Success) { "PASS" } else { "FAIL" }
    $TestSuite = $TestSuites[$Result.Key]
    
    $HtmlReport += @"
        <tr>
            <td>$($TestSuite.Name)</td>
            <td class="$StatusClass">$Status</td>
            <td>$($TestSuite.Description)</td>
        </tr>
"@
}

$HtmlReport += @"
    </table>
    
    <h2>Artifacts</h2>
    <ul>
        <li><a href="test_log.txt">Complete Test Log</a></li>
        <li><a href="test_summary.txt">Text Summary</a></li>
"@

foreach ($Result in $Results.GetEnumerator()) {
    if ($Result.Value.Success) {
        $HtmlReport += "        <li><a href=`"$($Result.Key)_results.txt`">$($TestSuites[$Result.Key].Name) Results</a></li>`n"
    }
    $HtmlReport += "        <li><a href=`"$($Result.Key)_error.txt`">$($TestSuites[$Result.Key].Name) Errors</a></li>`n"
}

$HtmlReport += @"
    </ul>
</body>
</html>
"@

$HtmlReport | Out-File -FilePath "$OutputDir\test_report.html"

# Final summary
Write-Log "`n=== Test Execution Complete ==="
Write-Log "Total Duration: $($Duration.TotalMinutes.ToString('F2')) minutes"
Write-Log "Results: $PassedCount passed, $FailedCount failed"
Write-Log "Success Rate: $([math]::Round($PassedCount / $Results.Count * 100, 1))%"
Write-Log "Reports generated in: $OutputDir"
Write-Log "HTML Report: $OutputDir\test_report.html"

if ($FailedCount -gt 0) {
    Write-Log "Some tests failed. Check error logs for details." "ERROR"
    exit 1
} else {
    Write-Log "All tests passed successfully!"
    exit 0
}
