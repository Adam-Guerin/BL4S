cmake_minimum_required(VERSION 3.16)
project(Geant4Test)

# Find Geant4
find_package(Geant4 REQUIRED)

# Add executable
add_executable(test_geant4 test_geant4.cpp)

# Link with Geant4 libraries
target_link_libraries(test_geant4 ${Geant4_LIBRARIES})
target_include_directories(test_geant4 PRIVATE ${Geant4_INCLUDE_DIRS})
