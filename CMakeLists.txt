cmake_minimum_required(VERSION 3.20)
project(pznet LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(JNI REQUIRED)

# ---- Pick arch suffix based on the generator/platform ----
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PZNET_ARCH "x64")
    set(RAKNET_NAME "RakNet64")
else()
    set(PZNET_ARCH "x86")
    set(RAKNET_NAME "RakNet32")
endif()

# ---- RakNet as an IMPORTED target ----
set(RAKNET_ROOT "${CMAKE_SOURCE_DIR}/third_party/raknet")

add_library(raknet SHARED IMPORTED GLOBAL )
set_target_properties(raknet PROPERTIES
        IMPORTED_LOCATION             "${RAKNET_ROOT}/bin/${PZNET_ARCH}/${RAKNET_NAME}.dll"
        IMPORTED_IMPLIB               "${RAKNET_ROOT}/lib/${PZNET_ARCH}/${RAKNET_NAME}.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${RAKNET_ROOT}/include"
)

# ---- Your library ----
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
        # raknet headers come in via the IMPORTED target
        # third_party/miniupnpc/include
        # third_party/opus/include
)

target_link_libraries(pznet PRIVATE
        raknet
        ${JNI_LIBRARIES}
)

set_target_properties(pznet PROPERTIES
        OUTPUT_NAME "ZNetNoSteam64"
        PREFIX ""
)

add_custom_command(TARGET pznet POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:raknet>"
        "$<TARGET_FILE_DIR:pznet>"
        COMMENT "Copying ${RAKNET_NAME}.dll to output directory"
)