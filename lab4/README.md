# Lab Variant 6 — Insertion Sort & Counting Sort + Graph (adjacency list)

## Overview
Project contains:
- Two template sorting algorithms:
  - `insertion_sort` — template, works with raw arrays `T[]` and `std::vector<T>`, supports custom comparator.
  - `counting_sort` — template that requires a key-extractor mapping elements to integer keys (works for built-in integral types with default extractor).
- Template `Graph<T, Traits>` — undirected graph implemented with adjacency lists. Provides:
  - vertex/edge add/remove, queries, iterators (vertex and edge iterators, adjacency iterators).
  - reverse and const variants.
  - exceptions on invalid operations.
- `Person` class as a sample user class with overloaded comparison operators.
- Unit tests using Google Test.

## Build & Run on Windows with vcpkg

### Requirements
- Visual Studio 2022 (with C++ development tools)
- CMake >= 3.14
- vcpkg (installed and integrated with Visual Studio)
- Google Test installed via vcpkg: `vcpkg install gtest:x64-windows`

### From command line (in Developer PowerShell for VS 2022)
```powershell
# Clean and create build directory
Remove-Item -Recurse -Force build -ErrorAction Ignore
mkdir build
cd build

# Configure with CMake
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="C:/Users/maksd/vcpkg/scripts/buildsystems/vcpkg.cmake"

# Build the project
cmake --build . --config Release

# Run tests
ctest -C Release --output-on-failure

# Run the demo application
.\src\Release\lab_main.exe