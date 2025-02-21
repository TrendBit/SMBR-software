#!/bin/bash

# Store the original directory
START_DIR=$(pwd)

# Ensure we return to the original directory on exit
trap 'cd "$START_DIR"' EXIT

# Create output directories if they do not exist
mkdir -p build/native

# Move into the build directory
cd build/native || exit 1  # Exit if cd fails

# Run build commands
cmake -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Release ../.. && \
make -j 8  && \
make install
