cmake_minimum_required(VERSION 4.2)
project(FunkinC__ VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# PACKAGES
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

add_executable(FunkinC__ funkin/src/main.cpp)

#LIBRARIES
target_link_libraries(FunkinC__ PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)