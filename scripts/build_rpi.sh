#!/bin/bash

# Store the original directory
START_DIR=$(pwd)

# Ensure we return to the original directory on exit
trap 'cd "$START_DIR"' EXIT

# Create output directories if they do not exist
mkdir -p build/rpi

# Move into the build directory
cd build/rpi || exit 1  # Exit if cd fails

#if argument is clean, remove build directory
rm -rf install/filesystem



# Run build commands
#set toolchain
#source /usr/local/smbr-kas/environment-setup-cortexa72-poky-linux
cmake -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/usr/local/smbr-kas/sysroots/x86_64-pokysdk-linux/usr/share/cmake/cortexa72-poky-linux-toolchain.cmake ../.. && \
make -j 8  && \
make install
