cmake_minimum_required(VERSION 3.20)
project(reactor-web-control-ts)

# Set source directory
set(WEB_CONTROL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Install the source files to target location
install(DIRECTORY ${WEB_CONTROL_SRC_DIR}/
    DESTINATION ${SMBR_INSTALL_HOME}/server-ts
    USE_SOURCE_PERMISSIONS
    PATTERN ".git" EXCLUDE
)

# Install dependencies and build on the target system
install(CODE "
    message(STATUS \"Installing server dependencies in \${CMAKE_INSTALL_PREFIX}/${SMBR_INSTALL_HOME}/server-ts/server\")
    execute_process(
        COMMAND npm install
        WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${SMBR_INSTALL_HOME}/server-ts/server
        RESULT_VARIABLE npm_server_result
    )
    if(NOT npm_server_result EQUAL 0)
        message(WARNING \"Server npm install failed with code \${npm_server_result}\")
    endif()

    message(STATUS \"Building server\")
    execute_process(
        COMMAND npm run build
        WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${SMBR_INSTALL_HOME}/server-ts/server
        RESULT_VARIABLE npm_server_build_result
    )
    if(NOT npm_server_build_result EQUAL 0)
        message(WARNING \"Server build failed with code \${npm_server_build_result}\")
    endif()

    message(STATUS \"Installing client dependencies in \${CMAKE_INSTALL_PREFIX}/${SMBR_INSTALL_HOME}/server-ts/client\")
    execute_process(
        COMMAND npm install
        WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${SMBR_INSTALL_HOME}/server-ts/client
        RESULT_VARIABLE npm_client_result
    )
    if(NOT npm_client_result EQUAL 0)
        message(WARNING \"Client npm install failed with code \${npm_client_result}\")
    endif()

    message(STATUS \"Building client\")
    execute_process(
        COMMAND npm run build
        WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}/${SMBR_INSTALL_HOME}/server-ts/client
        RESULT_VARIABLE npm_build_result
    )
    if(NOT npm_build_result EQUAL 0)
        message(WARNING \"Client build failed with code \${npm_build_result}\")
    endif()
")

# Install systemd service file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/reactor-web-control-ts.service
    DESTINATION ${SMBR_INSTALL_SERVICES}
)
