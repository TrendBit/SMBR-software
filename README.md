# SMBR-software
all software running on SMBR main unit (RPI)


## Requirements
- CMake 3.20 or later
- OATPP 1.4.0
- OATPP-Swagger 1.4.0
- SocketCAN
- Boost Asio

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
    ```
2. Create a build directory and compile the project:
    ```sh
    mkdir -p build && cd build
    cmake ..
    make
    ```
Then start with `./SMBR.Services/api-server/api-server`


## Verifying Functionality
1. Verify that the application is running on the correct port (default is 8089):
    Open a web browser on the device you are connected to via SSH and go to URL: `http://ip_address_of_rpi:8089/swagger/ui` and test endpoints.


## Structure

### SMBR (library)
Contains abstract interfaces

### SMBR.Modules.Virtual (library)
Dummy/Virtual implementation of abstract interfaces

### SMBR.REST (library)
oatpp based REST server implementation

### SMBR.Services/api-server (executable)
api server - REST service - contains main.cpp


## TODO

### SMBR.Modules.CAN
Implementation of SMBR intrfaces using CAN

### SMBR.Scheduler
Mapping YAML scipts to SMBR interfaces

