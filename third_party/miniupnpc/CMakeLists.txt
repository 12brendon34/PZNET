cmake_minimum_required(VERSION 3.14)

project(miniupnpc
        VERSION 2.3.3
        DESCRIPTION "UPnP IGD client lightweight library"
        LANGUAGES C)

set(MINIUPNPC_API_VERSION 21)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/miniupnpcstrings.h.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/miniupnpcstrings.h"
        @ONLY
)

set(MINIUPNPC_SOURCES
        src/addr_is_reserved.c
        src/connecthostport.c
        src/igd_desc_parse.c
        src/minisoap.c
        src/minissdpc.c
        src/miniupnpc.c
        src/miniwget.c
        src/minixml.c
        src/portlistingparse.c
        src/receivedata.c
        src/upnpcommands.c
        src/upnpdev.c
        src/upnperrors.c
        src/upnpreplyparse.c
)

add_library(miniupnpc STATIC ${MINIUPNPC_SOURCES})

set_target_properties(miniupnpc PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(miniupnpc
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
)

target_compile_definitions(miniupnpc PUBLIC MINIUPNP_STATICLIB)

if (WIN32)
    target_link_libraries(miniupnpc PUBLIC ws2_32 iphlpapi)

    target_compile_definitions(miniupnpc PRIVATE
            _CRT_SECURE_NO_WARNINGS
            _WINSOCK_DEPRECATED_NO_WARNINGS
            _WIN32_WINNT=0x0601
    )
else ()
    target_compile_definitions(miniupnpc PRIVATE
            MINIUPNPC_SET_SOCKET_TIMEOUT
            MINIUPNPC_GET_SRC_ADDR
            _BSD_SOURCE
            _DEFAULT_SOURCE
    )

    if (NOT APPLE
            AND NOT CMAKE_SYSTEM_NAME MATCHES ".*BSD"
            AND NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")
        target_compile_definitions(miniupnpc PRIVATE _XOPEN_SOURCE=600)
    endif ()

    if (APPLE)
        target_compile_definitions(miniupnpc PRIVATE _DARWIN_C_SOURCE)
    endif ()

    if (CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
        target_compile_definitions(miniupnpc PRIVATE _NETBSD_SOURCE)
    endif ()
endif ()

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(miniupnpc PRIVATE -Wall)
endif ()