# PROJECT
cmake_minimum_required(VERSION 4.2)
project(FunkinC__ VERSION 0.0.1 LANGUAGES CXX C)

# SET
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/export/$<CONFIG>")

# PACKAGES
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

# EXE
add_executable(FunkinC__ funkin/src/main.cpp)

# LIBRARIES
target_link_libraries(FunkinC__ PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)

# PROGRAMS
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "C:/Qt/6.11.1/mingw_64/bin")

# COMMANDS
if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET FunkinC__ POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} --qmldir "${CMAKE_SOURCE_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<TARGET_FILE_NAME:FunkinC__>"
            COMMENT "Deploying Qt dependencies"
    )
endif()