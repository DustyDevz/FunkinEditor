# FunkinC++ Engine
# Copyright (©) 2026 Dusty. All rights reserved.
#
# This project is licensed under the terms of the MIT license.
# For a copy, see <https://opensource.org/licenses/MIT>.

cmake_minimum_required(VERSION 4.2)
project(FunkinEditor VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/export/$<CONFIG>")

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
endif()

if(MINGW AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "ccache hooked successfully.")
endif()

file(GLOB_RECURSE ENGINE_SOURCES
        "funkin/src/*.cpp"
        "funkin/include/*.hpp"
        "funkin/qt/qrc/*.qrc"
)

add_subdirectory("funkin/libs/bgfx")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(Vulkan REQUIRED)

add_executable(FunkinEditor ${ENGINE_SOURCES})
set_target_properties(FunkinEditor PROPERTIES
        AUTOMOC ON
        AUTORCC ON
)

target_compile_definitions(FunkinEditor PRIVATE
        $<$<CONFIG:Debug>:FUNKIN_DEBUG>
        VK_USE_PLATFORM_WIN32_KHR
)

target_link_libraries(FunkinEditor PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Vulkan::Vulkan
        bgfx
)

target_include_directories(FunkinEditor PRIVATE
        "${CMAKE_SOURCE_DIR}/funkin/include"
        "${CMAKE_SOURCE_DIR}/funkin/libs/clay"
        "${CMAKE_SOURCE_DIR}/funkin/libs/bgfx/bgfx/include"
        "${CMAKE_SOURCE_DIR}/funkin/libs/bgfx/bx/include"
        "${CMAKE_SOURCE_DIR}/funkin/libs/bgfx/bimg/include"
)

target_precompile_headers(FunkinEditor PRIVATE
        "${CMAKE_SOURCE_DIR}/funkin/include/utils/pch.hpp"
)

get_target_property(_qt_core_location Qt6::Core IMPORTED_LOCATION_RELEASE)
get_filename_component(_qt_bin_dir "${_qt_core_location}" DIRECTORY)
get_filename_component(_qt_root_dir "${_qt_bin_dir}" DIRECTORY)

get_filename_component(_mingw_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)

add_custom_command(TARGET FunkinEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_mingw_bin_dir}/libwinpthread-1.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_mingw_bin_dir}/libgcc_s_seh-1.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_mingw_bin_dir}/libstdc++-6.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_bin_dir}/Qt6Core.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_bin_dir}/Qt6Gui.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_bin_dir}/Qt6Qml.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_bin_dir}/Qt6QmlMeta.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_bin_dir}/Qt6Quick.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_bin_dir}/Qt6OpenGL.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_bin_dir}/Qt6Network.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_bin_dir}/Qt6QmlModels.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_bin_dir}/Qt6QmlWorkerScript.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"

        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_root_dir}/plugins/platforms/qwindows.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms/"
)