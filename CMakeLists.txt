cmake_minimum_required(VERSION 3.12)

set(Darknet_MAJOR_VERSION 0)
set(Darknet_MINOR_VERSION 2)
set(Darknet_PATCH_VERSION 5)
set(Darknet_TWEAK_VERSION 3)
set(Darknet_VERSION ${Darknet_MAJOR_VERSION}.${Darknet_MINOR_VERSION}.${Darknet_PATCH_VERSION}.${Darknet_TWEAK_VERSION})

project(Darknet VERSION ${Darknet_VERSION})

message("${CMAKE_CURRENT_LIST_DIR}")
message("${CMAKE_MODULE_PATH}")

message("${INSTALL_INCLUDE_DIR}")
include(cmake/darknetOptions.cmake)
message("${INSTALL_INCLUDE_DIR}")

set(DARKNET_INSTALL_INCLUDE_DIR ${INSTALL_INCLUDE_DIR})
# Make relative paths absolute (needed later on)
foreach (p LIB BIN INCLUDE CMAKE)
    set(var INSTALL_${p}_DIR)
    if (NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif ()
endforeach ()

configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/include/version.h.in"
        "${CMAKE_CURRENT_LIST_DIR}/include/version.h"
)

#look for all *.h files in include folder
file(GLOB headers "${CMAKE_CURRENT_LIST_DIR}/include/*.h")
#add also files in the include folder
list(APPEND headers
        ${CMAKE_CURRENT_LIST_DIR}/include/darknet.h
        )
#add also *.hpp files
list(APPEND headers
        ${CMAKE_CURRENT_LIST_DIR}/include/realsense-opencv-helpers.hpp
        )
#remove windows only files
if (NOT MSVC)
    list(REMOVE_ITEM headers
            ${CMAKE_CURRENT_LIST_DIR}/include/gettimeofday.h
            ${CMAKE_CURRENT_LIST_DIR}/include/getopt.h
            )
endif ()
#set(exported_headers ${headers})

#look for all *.c files in src folder
file(GLOB sources "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
#add also .cpp files
list(APPEND sources
        ${CMAKE_CURRENT_LIST_DIR}/src/http_stream.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/image_opencv.cpp
        )
#remove darknet.c file which is necessary only for the executable, not for the lib
list(REMOVE_ITEM sources
        ${CMAKE_CURRENT_LIST_DIR}/src/darknet.c
        )
#remove windows only files
if (NOT MSVC)
    list(REMOVE_ITEM sources
            ${CMAKE_CURRENT_LIST_DIR}/src/gettimeofday.c
            ${CMAKE_CURRENT_LIST_DIR}/src/getopt.c
            )
endif ()

if (ENABLE_CUDA)
    file(GLOB cuda_sources "${CMAKE_CURRENT_LIST_DIR}/cuda/*.cu")
    message("WITH CUDA!!!")
else()
    message("NO CUDA!!!")
endif ()

if (BUILD_AS_CPP)
    set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)
endif ()

include_directories(dark include)
add_library(dark ${CMAKE_CURRENT_LIST_DIR}/include/yolo_v2_class.hpp ${CMAKE_CURRENT_LIST_DIR}/src/yolo_v2_class.cpp ${sources} ${headers} ${cuda_sources})
set_target_properties(dark PROPERTIES POSITION_INDEPENDENT_CODE ON)
if (ENABLE_CUDA)
    set_target_properties(dark PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif ()
if (BUILD_SHARED_LIBS)
    target_compile_definitions(dark PRIVATE LIB_EXPORTS=1)
endif ()
if (BUILD_AS_CPP)
    set_target_properties(dark PROPERTIES LINKER_LANGUAGE CXX)
endif ()
set_target_properties(dark PROPERTIES OUTPUT_NAME "darknet")

if (OpenCV_FOUND AND OpenCV_VERSION VERSION_GREATER "3.0" AND BUILD_USELIB_TRACK)
    include_directories(uselib_track include)
    add_executable(uselib_track ${CMAKE_CURRENT_LIST_DIR}/src/yolo_console_dll.cpp)
endif ()

include_directories(uselib include)
add_executable(uselib ${CMAKE_CURRENT_LIST_DIR}/src/yolo_console_dll.cpp)
if (BUILD_AS_CPP)
    set_target_properties(uselib PROPERTIES LINKER_LANGUAGE CXX)
endif ()

include_directories(darknet include)
message("${sources}")
message("${headers}")
message("${cuda_sources}")
add_executable(darknet ${CMAKE_CURRENT_LIST_DIR}/src/darknet.c ${sources} ${headers} ${cuda_sources})
if (BUILD_AS_CPP)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/src/darknet.c PROPERTIES LANGUAGE CXX)
    set_target_properties(darknet PROPERTIES LINKER_LANGUAGE CXX)
endif ()

target_include_directories(darknet PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src> $<INSTALL_INTERFACE:${DARKNET_INSTALL_INCLUDE_DIR}> $<BUILD_INTERFACE:${Stb_INCLUDE_DIR}>)
target_include_directories(dark PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src> $<INSTALL_INTERFACE:${DARKNET_INSTALL_INCLUDE_DIR}> $<BUILD_INTERFACE:${Stb_INCLUDE_DIR}>)
target_include_directories(uselib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src> $<INSTALL_INTERFACE:${DARKNET_INSTALL_INCLUDE_DIR}> $<BUILD_INTERFACE:${Stb_INCLUDE_DIR}>)

include(cmake/darknetLinkLibraries.cmake)

message("${INSTALL_BIN_DIR}")
message("${INSTALL_LIB_DIR}")
message("${INSTALL_INCLUDE_DIR}")
include(cmake/darknetInstall.cmake)
