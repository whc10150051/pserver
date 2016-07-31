cmake_minimum_required(VERSION 2.8)
############# Global settings ##################################################
# Turn on project folders feature
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Setup configuration types: Debug and Release
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )

############# Main project definition ##########################################

project(pserver)

############# toolchin #########################################################
set(CMAKE_C_COMPILER   /opt/encotes/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /opt/encotes/bin/arm-linux-gnueabihf-g++)
set(CMAKE_STRIP /opt/encotes/bin/arm-linux-gnueabihf-strip)

############# definitions#######################################################
#add_definitions(-std=c++11 -O2 -march=armv7-a -mtune=cortex-a8 -ftree-vectorize -mfpu=vfp -mfloat-abi=soft)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon")

############# modules #########################################################
add_subdirectory(src/modules)

set(PROJECT_LIB_DIR /opt/encotes/lib)

############# RPATH ############################################################
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_SKIP_RPATH FALSE) # use RPATH options
set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

############# Common third party libs ##########################################
# Path for cmake find lib modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/CMakeModules)

# Find POCO
find_package(Poco COMPONENTS Foundation JSON Net Util XML REQUIRED)

############# Common build output dir ##########################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})

############# Common includes ##################################################
include(${CMAKE_SOURCE_DIR}/cmake/BaseConfiguration.cmake)

############# Source code structure ############################################

set(SOURCES
        main.cpp
        src/Server.cpp
        src/Connection.cpp
        src/Voltage.cpp
        )

set(HEADERS
        src/Server.h
        src/utils/Logger.h
        src/Const.h
        src/Connection.h
        src/Voltage.h
        src/utils/ProductInfo.h
        )

set(SERVICE_SOURCES
        src/service/ServerApplication.cpp
        src/service/ServiceName.cpp
        )

set(SERVICE_HEADERS
        src/service/ServerApplication.h
        src/service/ServiceName.h
        )

set(SERVICE_FILES
        ${SERVICE_SOURCES}
        ${SERVICE_HEADERS}
        )

set(TASK_SOURCES
        src/tasks/AbstractTask.cpp
        src/tasks/FittingTask.cpp
        src/tasks/TaskFactory.cpp
        src/tasks/EchoTask.cpp
        src/tasks/StatusTask.cpp
        src/tasks/ScanResonanceTask.cpp
        src/tasks/UniversalTask.cpp
        src/tasks/MeasureTask.cpp
        )

set(TASK_HEADERS
        src/tasks/AbstractTask.h
        src/tasks/FittingTask.h
        src/tasks/TaskFactory.h
        src/tasks/EchoTask.h
        src/tasks/StatusTask.h
        src/tasks/ScanResonanceTask.h
        src/tasks/UniversalTask.h
        src/tasks/MeasureTask.h
        )

set(TASK_FILES
        ${TASK_SOURCES}
        ${TASK_HEADERS}
        )

set(UTIL_SOURCES
        src/utils/JsonHelper.cpp
        src/utils/LogConfiguration.cpp
        src/utils/StringHelper.cpp
        )

set(UTIL_HEADERS
        src/utils/JsonHelper.h
        src/utils/LogConfiguration.h
        src/utils/StringHelper.h
        src/utils/ProductInfo.h
        )

set(UTIL_FILES
        ${UTIL_SOURCES}
        ${UTIL_HEADERS}
        )

set(ALL_SOURCES
        ${SOURCES}
        ${HEADERS}
        ${UTIL_FILES}
        ${TASK_FILES}
        ${SERVICE_FILES}
        )

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})
source_group("Util Files" FILES ${UTIL_FILES})
source_group("Task Files" FILES ${TASK_FILES})
source_group("Service Files" FILES ${SERVICE_FILES})

############# Compiler includes ################################################
include_directories(${PROJECT_SOURCE_DIR}/)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/utils)
include_directories(${PROJECT_SOURCE_DIR}/src/service)
include_directories(${POCO_INCLUDE_DIRS})

include_directories(src/modules/common)
include_directories(src/modules/module_mng_device)
include_directories(src/modules/module_mng_device/src)
include_directories(src/modules/module_mng_device/emul)
include_directories(src/modules/module_of_measurements)
include_directories(src/modules/module_formation_signal)

############# Linker search path ###############################################
link_directories(${POCO_LIBRARY_DIR})
# поменять на PATH
link_directories(${PROJECT_LIB_DIR})

############# Target ###########################################################
add_executable(pserver ${ALL_SOURCES})

set_target_properties(pserver PROPERTIES OUTPUT_NAME pserver PREFIX "")

setup_target_output(pserver)

############# Target dependencies ##############################################
if (UNIX)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(pserver PocoFoundationd PocoJSONd PocoNetd PocoUtild PocoXMLd common module_formation_signal module_of_measurements module_mng_device pthread dl m)
    else()
        target_link_libraries(pserver PocoFoundation PocoJSON PocoNet PocoUtil PocoXML common module_formation_signal module_of_measurements module_mng_device pthread dl m)
    endif()
endif(UNIX)

############# Pre-build event ##################################################

add_custom_target(pserver_prebuild
        COMMAND sh ${CMAKE_SOURCE_DIR}/cmake/prebuild.sh ${CMAKE_SOURCE_DIR}/src/BuildInfo.h
        )
add_dependencies(pserver pserver_prebuild)
