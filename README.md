# SMBR-software
All software components running on SMBR main unit (RPI).
Contains:
    - API-server (including scheduler)
    - Web-control
    - Firmware tested with current software
    - Recipe examples
    - Database export configuration
    - Other tools (can-watch, avahi configuration, etc)

For testing is possible to build project for native architecture tgo deploy software onto SMPBR it must be cross compiled pro Main unit architecture (RPi4-arm64). Compiled project has same folder structure and can be directly synchronized with RPi filesystem. For building and synchronization, there are helper scripts inside `scripts` folder.

For cross compilation si required to install toolchain. This toolchain can be obtained from BSP repository.

# Structure
- `scripts` - scripts for building and installation
- `SMBR` - Library, contains abstract interfaces
- `SMBR.Virtual` - Library, Dummy/Virtual implementation of abstract interfaces
- `SMBR.Can.Codes` - Library, Basic CAN codes (git submodule)
- `SMBR.Can.Modules` - Real CAN module interface implementation
- `SMBR.Scheduler` - Library, Script parser and interpreter
- `SMBR.REST` - Library, Oatpp based REST server implementation
- `SMBR.Services`
    - `reactor-api-server` - Executable Api server - REST service - contains main.cpp
    - `reactor-core-module` - Executable virtual Core module connected to CAN
- `SMBR.Utilities`
    - `reactor-can-watch` - Executable, listen and interpret can messages
    - `reactor-script-api` - Executable, runs recipe from local file on local api server

# Deploy build / Crosscompilation
Working system with older version of software is assumed to be in local network

1. Clone this repository:
    ```sh
    git clone https://github.com/TrendBit/SMBR-software.git
    cd SMBR-software
    git submodule update --init --recursive
    ```
2. Install toolchain
    ```sh
    chmod +x poky-glibc-x86_64-smbr-image-cortexa72-smbr-toolchain-x.x.x.sh
    sudo ./poky-glibc-x86_64-smbr-image-cortexa72-smbr-toolchain-x.x.x.sh -d /usr/local/smbr-kas/
    ```

3. Build sources and upload to smbr
    ```sh
    ./scripts/update_rpi.sh [IP]
    ```

# Native build
This variant is mainly used for testing on local machine without real SMPBR device.

### Requirements
- CMake 3.20 or later
- OATPP 1.4.0
- OATPP-Swagger 1.4.0
- SocketCAN
- Poco

### Installing Dependencies for Native build
1. Install CMake:
    ```sh
    sudo apt install cmake
    ```
2. Install OATPP:
    ```sh
    git clone https://github.com/oatpp/oatpp
    cd oatpp
    mkdir build && cd build
    cmake ..
    make install
    ```
3. Install OATPP-Swagger:
    ```sh
    git clone https://github.com/oatpp/oatpp-swagger
    cd oatpp-swagger
    mkdir build && cd build
    cmake ..
    make install
    ```
4. Install SocketCAN:
    ```sh
    sudo apt install can-utils
    ```
5. Install Poco libraries:
    ```sh
    sudo apt-get install libpoco-dev
    ```

### Building the Project
1. Clone this repository:
    ```sh
    git clone https://github.com/TrendBit/SMBR-software.git
    cd SMBR-software
    git submodule update --init --recursive
    ```
2. Create a build directory and compile the project:
    ```sh
    ./scripts/build_native.sh
    ```
3. Then start with `./build/native/install/filesystem/usr/local/bin/smbr-api-server`

# Verifying Functionality
Verify that the application is running on the correct port (default is 8089). Open a web browser on the device you are connected to via SSH and go to URL: `http://ip_address_of_rpi:8089/swagger/ui` and test endpoints.
