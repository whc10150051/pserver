cmake_minimum_required(VERSION 2.8)

############# Global settings ##################################################
# Turn on project folders feature
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Setup configuration types: Debug and Release
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )

############# Main project definition ##########################################
project(modules)

############# Common build output dir ##########################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})

############# Include subdirs ##################################################
add_subdirectory(common)
add_subdirectory(module_formation_signal)
add_subdirectory(module_mng_device)
add_subdirectory(module_of_measurements)
