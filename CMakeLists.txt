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

set(DILIGENT_BUILD_CORE_COMBINED_LIB OFF CACHE BOOL "Disables combined library generation" FORCE)

file(GLOB_RECURSE ENGINE_SOURCES
        "funkin/src/*.cpp"
        "funkin/include/*.hpp"
)

add_subdirectory("funkin/external/DiligentEngine")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

add_executable(FunkinC__ ${ENGINE_SOURCES})

target_link_libraries(FunkinC__ PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
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
endif()