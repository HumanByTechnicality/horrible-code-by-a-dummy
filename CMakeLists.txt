cmake_minimum_required(VERSION 3.26)
project(newGame)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")

set(CMAKE_CXX_STANDARD 20)

add_executable(newGame main.cpp)

set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML/lib/cmake/SFML")

find_package(SFML COMPONENTS system network window audio graphics CONFIG REQUIRED)
target_link_libraries(newGame PUBLIC sfml-system sfml-network sfml-audio sfml-window sfml-graphics)
