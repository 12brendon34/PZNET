cmake_minimum_required(VERSION 3.20)
project(pznet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(JNI REQUIRED)

add_subdirectory(third_party/raknet)
add_subdirectory(third_party/miniupnpc EXCLUDE_FROM_ALL)

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
        RakNet
        miniupnpc
        ${JNI_LIBRARIES}
)
if (WIN32)
    target_link_libraries(pznet PRIVATE ws2_32 iphlpapi)
    target_compile_definitions(pznet PRIVATE MINIUPNP_STATICLIB _WIN32_WINNT=0x0601)
endif ()

set_target_properties(pznet PROPERTIES
        OUTPUT_NAME "ZNetNoSteam64"
)