cmake_minimum_required(VERSION 3.20)
project(pznet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(JNI REQUIRED)

set(RAKNET_ROOT "${CMAKE_SOURCE_DIR}/third_party/raknet")

if (WIN32)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(RAKNET_ARCH_DIR "x64")
        set(RAKNET_LIB_NAME "RakNet64")
    else ()
        set(RAKNET_ARCH_DIR "x86")
        set(RAKNET_LIB_NAME "RakNet32")
    endif ()

    add_library(raknet SHARED IMPORTED GLOBAL)
    set_target_properties(raknet PROPERTIES
            IMPORTED_LOCATION "${RAKNET_ROOT}/bin/${RAKNET_ARCH_DIR}/${RAKNET_LIB_NAME}.dll"
            IMPORTED_IMPLIB "${RAKNET_ROOT}/lib/${RAKNET_ARCH_DIR}/${RAKNET_LIB_NAME}.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${RAKNET_ROOT}/include"
    )

elseif (UNIX AND NOT APPLE)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
        set(RAKNET_ARCH_DIR "AArch64")
        set(RAKNET_LIB_NAME "libRakNet64.so")
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(RAKNET_ARCH_DIR "x64")
        set(RAKNET_LIB_NAME "libRakNet64.so")
    else ()
        # set(RAKNET_ARCH_DIR "x86")
        # set(RAKNET_LIB_NAME "libRakNet32.so")

        message(FATAL_ERROR "still need to find a 32-bit Linux RakNet")
    endif ()

    add_library(raknet SHARED IMPORTED GLOBAL)
    set_target_properties(raknet PROPERTIES
            IMPORTED_LOCATION "${RAKNET_ROOT}/bin/${RAKNET_ARCH_DIR}/${RAKNET_LIB_NAME}"
            INTERFACE_INCLUDE_DIRECTORIES "${RAKNET_ROOT}/include"
    )

else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()


add_library(pznet SHARED
        src/ZNetLog.cpp
        src/ZNetLog.h
        src/zombie_core_raknet_RakNetPeerInterface.cpp
        src/zombie_core_raknet_RakVoice.cpp
        src/zombie_core_znet_PortMapper.cpp
        src/zombie_core_znet_ZNet.cpp
        src/jnicommon.cpp
        src/jnicommon.h
)

target_include_directories(pznet PRIVATE
        include
        ${JNI_INCLUDE_DIRS}
)

target_link_libraries(pznet PRIVATE
        raknet
        ${JNI_LIBRARIES}
)

set_target_properties(pznet PROPERTIES
        OUTPUT_NAME "ZNetNoSteam64"
)