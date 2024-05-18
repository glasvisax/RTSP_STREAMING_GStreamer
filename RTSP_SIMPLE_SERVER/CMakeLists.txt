cmake_minimum_required(VERSION 3.14)
project(RTSP_SIMPLE_SERVER)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)

set(GSTREAMER_ROOT_DIR D:/gstreamer/install) 

find_package(PkgConfig REQUIRED)

set(ENV{PKG_CONFIG_PATH} ${GSTREAMER_ROOT_DIR}/lib/pkgconfig)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)
pkg_search_module(gstreamer-rtsp-server REQUIRED IMPORTED_TARGET gstreamer-rtsp-server-1.0>=1.4)
pkg_search_module(gstreamer-rtsp REQUIRED IMPORTED_TARGET gstreamer-rtsp-1.0>=1.4)
pkg_search_module(gstreamer-rtp REQUIRED IMPORTED_TARGET gstreamer-rtp-1.0>=1.4)

add_executable(server src/main.c src/Server.c src/Server.h)

target_link_libraries(server
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-video
    PkgConfig::gstreamer-rtsp-server
    PkgConfig::gstreamer-rtsp
    PkgConfig::gstreamer-rtp
)


