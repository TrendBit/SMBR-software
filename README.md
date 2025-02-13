# SMBR-software
all software running on SMBR main unit (RPI)

## Requirements
- CMake 3.20 or later
- OATPP 1.4.0
- OATPP-Swagger 1.4.0
- SocketCAN
- Poco

## Installing Dependencies
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

## Building the Project
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

## Crosscompilation
1. Flash image to SD card 
    ```sh
    wget https://trendbit.cz/downloads/smbr/smbr-image-smbr.rootfs.wic
    ```
2. Install toolchain
    ```sh
    wget https://trendbit.cz/downloads/smbr/poky-glibc-x86_64-smbr-image-cortexa72-smbr-toolchain-5.0.6.sh
    chmod +x poky-glibc-x86_64-smbr-image-cortexa72-smbr-toolchain-5.0.6.sh
    sudo ./poky-glibc-x86_64-smbr-image-cortexa72-smbr-toolchain-5.0.6.sh -d /usr/local/smbr-kas/
    ```

3. Build sources and upload to smbr
    ```sh
    ./scripts/update_rpi.sh [IP]
    ./scripts/enable_services_rpi.sh [IP]
    ./scripts/update_rpi.sh [IP]
    ```


## Verifying Functionality
1. Verify that the application is running on the correct port (default is 8089):
    Open a web browser on the device you are connected to via SSH and go to URL: `http://ip_address_of_rpi:8089/swagger/ui` and test endpoints.


## Structure

### scripts
scripts for building and installation

### SMBR (library)
Contains abstract interfaces

### SMBR.Virtual (library)
Dummy/Virtual implementation of abstract interfaces

### SMBR.Can.Codes (library)
basic can codes library (git submodule)

### SMBR.Can.Modules (library)
real can module interface implementation

### SMBR.Scheduler (library)
script parser and interpreter

### SMBR.REST (library)
oatpp based REST server implementation

### SMBR.Services
#### smbr-api-server (executable)
api server - REST service - contains main.cpp

#### smbr-core-module (executable)
core can module

#### other services ... (to be done)


