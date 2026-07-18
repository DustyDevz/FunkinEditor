# FunkinC++ Engine
# Copyright (©) 2026 Dusty. All rights reserved.
#
# This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
# For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.

cmake_minimum_required(VERSION 4.2)
project(FunkinEditor VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/export/$<CONFIG>")

set(DILIGENT_BUILD_TOOLS ON CACHE BOOL "Build DiligentTools module" FORCE)
set(DILIGENT_BUILD_FX OFF CACHE BOOL "Build DiligentFX module" FORCE)
set(DILIGENT_BUILD_SAMPLES OFF CACHE BOOL "Build DiligentSamples module" FORCE)
set(DILIGENT_BUILD_DOCS OFF CACHE BOOL "Build documentation" FORCE)

set(DILIGENT_NO_DIRECT3D11 ON CACHE BOOL "Disable Direct3D11" FORCE)
set(DILIGENT_NO_DIRECT3D12 ON CACHE BOOL "Disable Direct3D12" FORCE)
set(DILIGENT_NO_VULKAN OFF CACHE BOOL "Enable Vulkan" FORCE)
set(DILIGENT_NO_GL OFF CACHE BOOL "Enable OpenGL/GLES" FORCE)

add_subdirectory("funkin/libs/diligent")

if(WIN32 AND TARGET Diligent-Win32Platform)
    target_compile_options(Diligent-Win32Platform PRIVATE
            -include "${CMAKE_SOURCE_DIR}/funkin/cmake/thread_desc_compat.h"
    )
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
        "funkin/src/*.cpp"
        "funkin/include/*.hpp"
)
add_executable(FunkinEditor ${ENGINE_SOURCES})

target_link_libraries(FunkinEditor PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Vulkan::Vulkan
        Diligent-GraphicsEngineVk-static
)

target_include_directories(FunkinEditor PRIVATE
        "${CMAKE_SOURCE_DIR}/funkin/include"
)