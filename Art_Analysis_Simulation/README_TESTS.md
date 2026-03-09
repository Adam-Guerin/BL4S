# Art Analysis Simulation - Test Suite

This directory contains comprehensive test suites for the Art Analysis Simulation framework. The tests ensure the reliability, accuracy, and performance of the physics simulation and analysis algorithms.

## Test Categories

### 1. Unit Tests (`unit_tests.cc`)
**Purpose**: Test individual functions and methods in isolation
- Material property calculations (Z, density)
- Historical pigment detection
- Stopping power calculations
- Thickness estimation algorithms
- Basic mathematical functions

**Usage**: 
```bash
./unit_tests
```

### 2. Integration Tests (`integration_tests.cc`)
**Purpose**: Test end-to-end simulation workflows
- Full simulation pipeline
- Multi-material analysis
- Detector response chains
- Energy loss validation
- Reconstruction algorithms
- Data export/import functionality

**Usage**:
```bash
./integration_tests
```

### 3. Performance Tests (`performance_tests.cc`)
**Purpose**: Benchmark system performance and identify bottlenecks
- Simulation speed (events/second)
- Memory usage analysis
- Reconstruction performance
- I/O performance
- Scalability testing
- Benchmark comparison

**Usage**:
```bash
./performance_tests
```

### 4. Physics Validation Tests (`physics_validation_tests.cc`)
**Purpose**: Validate physics models against theoretical expectations
- Bethe-Bloch formula implementation
- Multiple Coulomb Scattering (Highland formula)
- Energy straggling
- Range calculations
- Bremsstrahlung radiation
- Pair production
- Compton scattering
- Photoelectric effect
- Conservation laws (energy, momentum)

**Usage**:
```bash
./physics_validation_tests
```

### 5. Regression Tests (`regression_tests.cc`)
**Purpose**: Ensure algorithm changes don't break existing functionality
- Material Z calculation regression
- Density calculation regression
- Stopping power regression
- Scattering angle regression
- Thickness estimation regression
- Transmission calculation regression
- Energy loss computation regression
- Angular resolution regression
- Reconstruction algorithm regression

**Usage**:
```bash
./regression_tests
# Create new baseline:
./regression_tests --create-baseline
```

## Quick Start

### Running All Tests
The easiest way to run all tests is using the automated test runner:

```powershell
# Run all tests with full reporting
.\run_all_tests.ps1

# Quick test (unit tests only)
.\run_all_tests.ps1 -Quick

# Run specific test categories
.\run_all_tests.ps1 -Performance
.\run_all_tests.ps1 -Physics
.\run_all_tests.ps1 -Integration
.\run_all_tests.ps1 -Regression
.\run_all_tests.ps1 -Unit
```

### Building Tests Manually
```bash
# Create build directory
mkdir build_tests && cd build_tests

# Configure with CMake
cmake .. -f ../test_cmake_lists.txt -DCMAKE_BUILD_TYPE=Release

# Build all tests
cmake --build . --config Release

# Run specific test
./Release/unit_tests
./Release/integration_tests
./Release/performance_tests
./Release/physics_validation_tests
./Release/regression_tests
```

### Using CTest
```bash
# Run all tests via CTest
ctest --output-on-failure

# Run specific test categories
ctest -L unit
ctest -L integration
ctest -L performance
ctest -L physics
ctest -L regression
```

## Test Configuration

### Environment Variables
- `GEANT4_DATA_DIR`: Path to Geant4 data files
- `ROOTSYS`: ROOT installation directory (optional)
- `HDF5_DIR`: HDF5 installation directory (optional)

### Build Types
- **Release**: Optimized build for performance testing
- **Debug**: Debug build with additional checks
- **RelWithDebInfo**: Optimized with debug symbols

### Test Parameters
Many tests can be configured by modifying the reference values at the top of each test file:

- `TOLERANCE`: Acceptable deviation for physics validation (default: 5%)
- `REGRESSION_TOLERANCE`: Acceptable deviation for regression tests (default: 2%)
- Event counts, energies, materials can be customized per test

## Output and Reports

### Test Results Location
All test results are saved to the `test_results/` directory by default:

```
test_results/
├── test_log.txt              # Complete execution log
├── test_summary.txt          # Text summary of results
├── test_report.html          # Detailed HTML report
├── unit_tests_results.txt    # Unit test output
├── integration_tests_results.txt
├── performance_tests_results.txt
├── physics_validation_tests_results.txt
├── regression_tests_results.txt
├── benchmark_report.txt      # Performance benchmark
└── regression_baseline.txt   # Regression reference values
```

### HTML Report
The HTML report provides:
- Test execution summary
- Individual test results with pass/fail status
- Links to detailed output files
- Performance metrics and benchmarks
- Error logs for failed tests

### Performance Benchmarks
Performance tests generate a `benchmark_report.txt` with:
- Simulation speed (events/second)
- Memory usage per event
- Reconstruction timing
- I/O performance metrics
- System information

## Continuous Integration

### GitHub Actions Example
```yaml
name: Test Suite
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Setup Geant4
      run: |
        # Install Geant4 dependencies
    - name: Build Tests
      run: |
        mkdir build_tests && cd build_tests
        cmake .. -f ../test_cmake_lists.txt
        cmake --build .
    - name: Run Tests
      run: |
        cd build_tests
        ctest --output-on-failure
    - name: Upload Results
      uses: actions/upload-artifact@v2
      with:
        name: test-results
        path: test_results/
```

### Test Coverage
To generate test coverage reports:
```bash
# Build with coverage flags
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON

# Run tests
ctest

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report/
```

## Troubleshooting

### Common Issues

1. **Geant4 Not Found**
   ```
   Error: Could not find Geant4
   ```
   **Solution**: Ensure Geant4 is installed and GEANT4_DIR is set correctly

2. **Missing Data Files**
   ```
   Error: Physics list not found
   ```
   **Solution**: Set GEANT4_DATA_DIR environment variable

3. **Build Failures**
   ```
   Error: Compilation failed
   ```
   **Solution**: Check compiler compatibility and required dependencies

4. **Test Timeouts**
   ```
   Error: Test timed out
   ```
   **Solution**: Increase timeout values or reduce event counts for performance tests

### Debug Mode
For detailed debugging:
```bash
# Build with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_DEBUG=ON

# Run with verbose output
./unit_tests --verbose
```

### Log Analysis
Check the test log for detailed error information:
```bash
# View complete test log
cat test_results/test_log.txt

# View specific test errors
cat test_results/unit_tests_error.txt
```

## Contributing

When adding new tests:

1. **Follow Naming Conventions**: Use descriptive function names like `test_specific_functionality()`
2. **Add Documentation**: Include comments explaining test purpose and expected results
3. **Update CMakeLists.txt**: Add new test executables and CTest entries
4. **Update README**: Document new test category and usage
5. **Set Timeouts**: Configure appropriate timeout values for long-running tests
6. **Add Labels**: Use CTest labels for categorization

### Test Development Guidelines
- Use `assert()` for validation checks
- Include both positive and negative test cases
- Test edge cases and boundary conditions
- Verify error handling and exception cases
- Keep tests independent and repeatable
- Use mock objects where appropriate for isolation

## Performance Targets

### Expected Performance Metrics
- **Unit Tests**: < 30 seconds
- **Integration Tests**: < 5 minutes
- **Performance Tests**: < 15 minutes
- **Physics Validation**: < 5 minutes
- **Regression Tests**: < 2 minutes

### Benchmark Targets
- **Simulation Speed**: > 100 events/second (simple geometry)
- **Memory Usage**: < 10 KB per event
- **Reconstruction Time**: < 30 seconds for 10k events
- **I/O Performance**: < 5 seconds for 10k events export

## License

This test suite is part of the Art Analysis Simulation project and follows the same licensing terms as the main project.
