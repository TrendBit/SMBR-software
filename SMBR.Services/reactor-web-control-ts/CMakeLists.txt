cmake_minimum_required(VERSION 3.20)
project(reactor-web-control-ts)

set(WEB_CONTROL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(WEB_CONTROL_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)

# Build the Node.js application during CMake build
add_custom_target(reactor-web-control-ts-build ALL
    COMMENT "Building reactor-web-control-ts"

    # Install and build server
    COMMAND ${CMAKE_COMMAND} -E echo "Installing server dependencies..."
    COMMAND cd ${WEB_CONTROL_SRC_DIR}/server && npm install
    COMMAND ${CMAKE_COMMAND} -E echo "Building server..."
    COMMAND cd ${WEB_CONTROL_SRC_DIR}/server && npm run build

    # Install and build client
    COMMAND ${CMAKE_COMMAND} -E echo "Installing client dependencies..."
    COMMAND cd ${WEB_CONTROL_SRC_DIR}/client && npm install
    COMMAND ${CMAKE_COMMAND} -E echo "Building client..."
    COMMAND cd ${WEB_CONTROL_SRC_DIR}/client && npm run build

    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install everything (source + built files + node_modules)
install(DIRECTORY ${WEB_CONTROL_SRC_DIR}/
    DESTINATION ${SMBR_INSTALL_HOME}/server-ts
    USE_SOURCE_PERMISSIONS
)

# Install systemd service file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/reactor-web-control-ts.service
    DESTINATION ${SMBR_INSTALL_SERVICES}
)
