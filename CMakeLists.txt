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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -ffast-math")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -ffast-math")


############# RPATH ############################################################
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_SKIP_RPATH FALSE) # use RPATH options
set(CMAKE_INSTALL_RPATH ${PROJECT_LIB_DIR})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

############# Common third party libs ##########################################
# Path for cmake find lib modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/CMakeModules)

# Find POCO
find_package(Poco COMPONENTS Foundation JSON Net Util XML REQUIRED)

############# Common includes ##################################################
include(${CMAKE_SOURCE_DIR}/cmake/BaseConfiguration.cmake)

set(SOURCE_FILES
    src/service/ServiceName.cpp
    src/service/ServiceApplication.cpp
    src/service/ServiceName.h
    src/service/ServiceApplication.h
    src/modules/common/const.h
    src/modules/common/convertors.cpp
    src/modules/common/convertors.h
    src/modules/common/error.h
    src/modules/common/in5102_types.cpp
    src/modules/common/in5102_types.h
    src/modules/common/log.cpp
    src/modules/common/log.h
    src/modules/common/utils.cpp
    src/modules/common/utils.h
    src/modules/module_formation_signal/module_formation_signal.cpp
    src/modules/module_formation_signal/module_formation_signal.h
    src/modules/module_mng_device/emul/basemeasuredevice.h
    src/modules/module_mng_device/emul/multiplexer.cpp
    src/modules/module_mng_device/emul/multiplexer.h
    src/modules/module_mng_device/emul/muxchannel.cpp
    src/modules/module_mng_device/emul/muxchannel.h
    src/modules/module_mng_device/emul/muxrelay.cpp
    src/modules/module_mng_device/emul/muxrelay.h
    src/modules/module_mng_device/emul/termalsensor.cpp
    src/modules/module_mng_device/emul/termalsensor.h
    src/modules/module_mng_device/emul/virtmeasuredevice.cpp
    src/modules/module_mng_device/emul/virtmeasuredevice.h
    src/modules/module_mng_device/src/buffer.cpp
    src/modules/module_mng_device/src/buffer.h
    src/modules/module_mng_device/src/commands.cpp
    src/modules/module_mng_device/src/commands.h
    src/modules/module_mng_device/src/commutator.cpp
    src/modules/module_mng_device/src/commutator.h
    src/modules/module_mng_device/src/in5102_lim.h
    src/modules/module_mng_device/src/in5201_lim.h
    src/modules/module_mng_device/src/measure_device.cpp
    src/modules/module_mng_device/src/measure_device.h
    src/modules/module_mng_device/src/packet.cpp
    src/modules/module_mng_device/src/packet.h
    src/modules/module_mng_device/src/spi.cpp
    src/modules/module_mng_device/src/spi.h
    src/modules/module_mng_device/src/test_fun.cpp
    src/modules/module_mng_device/src/test_fun.h
    src/modules/module_mng_device/src/uart.cpp
    src/modules/module_mng_device/src/uart.h
    src/modules/module_mng_device/module_mng_device.cpp
    src/modules/module_mng_device/module_mng_device.h
    src/modules/module_of_measurements/module_of_measurements.cpp
    src/modules/module_of_measurements/module_of_measurements.h
    src/tasks/AbstractTask.cpp
    src/tasks/AbstractTask.h
    src/tasks/EchoTask.cpp
    src/tasks/EchoTask.h
    src/tasks/FittingTask.cpp
    src/tasks/FittingTask.h
    src/tasks/MeasureTask.cpp
    src/tasks/MeasureTask.h
    src/tasks/ScanResonanceTask.cpp
    src/tasks/ScanResonanceTask.h
    src/tasks/StatusTask.cpp
    src/tasks/StatusTask.h
    src/tasks/TaskFactory.cpp
    src/tasks/TaskFactory.h
    src/tasks/UniversalTask.cpp
    src/tasks/UniversalTask.h
    src/utils/JsonHelper.cpp
    src/utils/JsonHelper.h
    src/utils/LogConfiguration.cpp
    src/utils/LogConfiguration.h
    src/utils/Logger.h
    src/utils/ProductInfo.h
    src/utils/StringHelper.cpp
    src/utils/StringHelper.h
    src/BuildInfo.h
    src/Connection.cpp
    src/Connection.h
    src/Const.h
    src/Server.cpp
    src/Server.h
    src/Voltage.cpp
    src/Voltage.h
    main.cpp)

################################################################################
include_directories(${PROJECT_SOURCE_DIR}/)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/utils)
include_directories(${PROJECT_SOURCE_DIR}/src/service)
include_directories(${PROJECT_SOURCE_DIR}/src/tasks)
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
add_executable(pserver ${SOURCE_FILES})

set_target_properties(pserver PROPERTIES OUTPUT_NAME pserver PREFIX "")

setup_target_output(pserver)

############# Target dependencies ##############################################
if (UNIX)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(pserver PocoFoundationd PocoJSONd PocoNetd PocoUtild PocoXMLd pthread dl m)
    else()
        target_link_libraries(pserver PocoFoundation PocoJSON PocoNet PocoUtil PocoXML pthread dl m)
    endif()
endif(UNIX)
