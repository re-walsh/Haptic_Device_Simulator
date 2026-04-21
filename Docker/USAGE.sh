#!/bin/bash
# Quick reference for building and using the Docker development environment

# 1. BUILD THE CONTAINER (one-time setup)
# From the project root directory:
docker compose -f Docker/docker-compose.yml build

# 2. START THE DEVELOPMENT CONTAINER
# This drops you into an interactive bash shell inside the container
docker compose -f Docker/docker-compose.yml up -d

# 3. ENTER THE RUNNING CONTAINER
docker compose -f Docker/docker-compose.yml exec cpp-dev-linux-x86 bash

# 4. BUILD YOUR C++ LIBRARY (inside the container)
cd /workspace/Code
mkdir -p build
cd build
cmake --preset linux-x64-cross ..
cmake --build . --config Release

# 5. RUN TESTS (if available)
ctest --output-on-failure

# 6. VIEW BUILD ARTIFACTS
# Check /workspace/Code/HapticDevice/bin/ for compiled libraries
# Check /workspace/Code/HapticDevice/lib/ for archives

# 7. STOP THE CONTAINER
docker compose -f Docker/docker-compose.yml down

# 8. CLEAN UP (remove images and volumes)
docker compose -f Docker/docker-compose.yml down -v

# NOTES:
# - The container uses Rosetta 2 emulation on M1 Macs to run x86_64 Linux
# - Your Code directory is mounted as a volume, so edits on macOS are reflected in the container
# - vcpkg dependencies are cached in a named volume for faster rebuilds
# - The preset "linux-x64-cross" is configured in your CMakePresets.json
