cmake_minimum_required(VERSION 3.14)
project(RTSP_SIMPLE_CLIENT)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)
set(GSTREAMER_ROOT_DIR D:/gstreamer/install) 
set(GTK3_ROOT_DIR C:/gtk-build/gtk/x64/release) 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS" )
find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH} ${GTK3_ROOT_DIR}/lib/pkgconfig)
pkg_check_modules(gtk3 REQUIRED IMPORTED_TARGET gtk+-3.0)

set(ENV{PKG_CONFIG_PATH} ${GSTREAMER_ROOT_DIR}/lib/pkgconfig)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)
pkg_search_module(gstreamer-rtsp-server REQUIRED IMPORTED_TARGET gstreamer-rtsp-server-1.0>=1.4)
pkg_search_module(gstreamer-rtsp REQUIRED IMPORTED_TARGET gstreamer-rtsp-1.0>=1.4)
pkg_search_module(gstreamer-rtp REQUIRED IMPORTED_TARGET gstreamer-rtp-1.0>=1.4)

file(GLOB_RECURSE SOURCES "src//*.c" "src//*.h")

add_executable(client ${SOURCES})

target_link_libraries(client 
    PkgConfig::gtk3
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-video
    PkgConfig::gstreamer-rtsp-server
    PkgConfig::gstreamer-rtsp
    PkgConfig::gstreamer-rtp
)


