# FunkinC++ Engine
# Copyright (©) 2026 Dusty. All rights reserved.
#
# This project is licensed under the terms of the MIT license.
# For a copy, see <https://opensource.org/licenses/MIT>.

cmake_minimum_required(VERSION 4.2)
project(FunkinC__ VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/export/$<CONFIG>")

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

if(WIN32 AND TARGET Diligent-Win32Platform)
    target_compile_options(Diligent-Win32Platform PRIVATE
            -include "${CMAKE_SOURCE_DIR}/funkin/cmake/compat/win32_thread_desc_compat.h"
    )
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(Vulkan REQUIRED)

add_executable(FunkinC__ ${ENGINE_SOURCES})
set_target_properties(FunkinC__ PROPERTIES
        AUTOMOC ON
        AUTORCC ON
)

target_compile_definitions(FunkinC__ PRIVATE
        $<$<CONFIG:Debug>:FUNKIN_DEBUG>
        VK_USE_PLATFORM_WIN32_KHR
)

target_link_libraries(FunkinC__ PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Vulkan::Vulkan
        Diligent-GraphicsEngineOpenGL-static
        Diligent-GraphicsEngineVk-static
        Diligent-GraphicsTools
        Diligent-GraphicsEngine
        Diligent-Primitives
)

target_include_directories(FunkinC__ PRIVATE "${CMAKE_SOURCE_DIR}/funkin/include")
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "C:/Qt/6.11.1/mingw_64/bin")

if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET FunkinC__ POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} --qmldir "${CMAKE_SOURCE_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<TARGET_FILE_NAME:FunkinC__>"
            COMMENT "Deploying Qt dependencies"
    )
    get_filename_component(COMPILER_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    add_custom_command(TARGET FunkinC__ POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COMPILER_BIN_DIR}/libstdc++-6.dll"
            "${COMPILER_BIN_DIR}/libgcc_s_seh-1.dll"
            "${COMPILER_BIN_DIR}/libwinpthread-1.dll"
            "$<TARGET_FILE_DIR:FunkinC__>"
            COMMENT "Overriding runtime DLLs with matching compiler toolchain"
    )
endif()