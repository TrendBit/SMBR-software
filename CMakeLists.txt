cmake_minimum_required(VERSION 3.20)

#build libraries in subdirectories
add_subdirectory(SMBR)
#add_subdirectory(SMBR.Modules.Can)
add_subdirectory(SMBR.Modules.Virtual)
add_subdirectory(SMBR.REST)


add_subdirectory(SMBR.Services/api-server)
