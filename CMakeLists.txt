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

set(DILIGENT_NO_METAL ON CACHE BOOL "Disable Metal" FORCE)
set(DILIGENT_NO_WEBGPU ON CACHE BOOL "Disable WebGPU" FORCE)
set(DILIGENT_NO_ARCHIVER ON CACHE BOOL "Disable Archiver" FORCE)
set(DILIGENT_NO_RENDER_STATE_PACKAGER ON CACHE BOOL "Disable Packager" FORCE)

set(DILIGENT_BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

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
        Diligent-GraphicsEngine
)

if(MSVC)
    set_property(TARGET FunkinEditor APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " /INCREMENTAL:NO /PDBALTPATH:%_PDB%")
    set_property(TARGET FunkinEditor APPEND_STRING PROPERTY LINK_FLAGS_MINSIZEREL " /INCREMENTAL:NO /PDBALTPATH:%_PDB%")
endif()

target_include_directories(FunkinEditor PRIVATE
        "${CMAKE_SOURCE_DIR}/funkin/include"
)

get_target_property(_qt_core_location Qt6::Core IMPORTED_LOCATION_RELEASE)
get_filename_component(_qt_bin_dir "${_qt_core_location}" DIRECTORY)
get_filename_component(_qt_root_dir "${_qt_bin_dir}" DIRECTORY)

set(COMPILER_DEPENDENT_DLL_COMMANDS "")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW)
    get_filename_component(_mingw_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set(COMPILER_DEPENDENT_DLL_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_mingw_bin_dir}/libwinpthread-1.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_mingw_bin_dir}/libgcc_s_seh-1.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_mingw_bin_dir}/libstdc++-6.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
    )
endif()

set(QT_CORE_DLLS
        Qt6Core.dll Qt6Gui.dll Qt6Qml.dll Qt6QmlMeta.dll Qt6Quick.dll
        Qt6OpenGL.dll Qt6Network.dll Qt6QmlModels.dll Qt6QmlWorkerScript.dll
)

set(QT_COPY_COMMANDS "")
foreach(_dll ${QT_CORE_DLLS})
    list(APPEND QT_COPY_COMMANDS COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_bin_dir}/${_dll}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
endforeach()

add_custom_command(TARGET FunkinEditor POST_BUILD
        ${COMPILER_DEPENDENT_DLL_COMMANDS}
        ${QT_COPY_COMMANDS}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_qt_root_dir}/plugins/platforms/qwindows.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms/"
        COMMAND "${_qt_bin_dir}/windeployqt.exe"
        --qmldir "${CMAKE_SOURCE_DIR}"
        --verbose 1
        --no-compiler-runtime
        --no-translations
        --no-opengl-sw
        "$<TARGET_FILE:FunkinEditor>"
)