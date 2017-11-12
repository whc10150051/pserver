include(CMakeForceCompiler)
set(CMAKE_SYSTEM_NAME pserver)
set(CMAKE_SYSTEM_PROCESSOR cortex-a8)
 
find_program(ARM_CC arm-linux-gnueabihf-gcc
    ${TOOLCHAIN_DIR}/bin
    )
find_program(ARM_CXX arm-linux-gnueabihf-g++
    ${TOOLCHAIN_DIR}/bin
    )
find_program(ARM_OBJCOPY arm-linux-gnueabihf-objcopy
    ${TOOLCHAIN_DIR}/bin
    )
find_program(ARM_SIZE_TOOL arm-linux-gnueabihf-size
    ${TOOLCHAIN_DIR}/bin)
 
CMAKE_FORCE_C_COMPILER(${ARM_CC} GNU)
CMAKE_FORCE_CXX_COMPILER(${ARM_CXX} GNU)
#
#set(CMAKE_C_FLAGS
#        "${CMAKE_C_FLAGS}"
#        "-std=c++11"
#        )
#
#set(CMAKE_CXX_FLAGS
#        "${CMAKE_CXX_FLAGS}"
#        "-std=c++11"
#        )

#if (CMAKE_SYSTEM_PROCESSOR STREQUAL "cortex-a8")
#
#  set(CMAKE_C_FLAGS
#    "${CMAKE_C_FLAGS}"
#    "-mcpu=cortex-a8 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -ffast-math -mthumb"
#  )
#
#  set(CMAKE_CXX_FLAGS
#    "${CMAKE_CXX_FLAGS}"
#    "-mcpu=cortex-a8 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -ffast-math -mthumb"
#  )
#
#else ()
#  message(WARNING
#    "Processor not recognised in toolchain file, "
#    "compiler flags not configured."
#  )
#endif ()
 
# fix long strings (CMake appends semicolons)
#string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
#
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")
#
#set(BUILD_SHARED_LIBS ON)
