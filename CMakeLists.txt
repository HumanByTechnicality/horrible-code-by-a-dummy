cmake_minimum_required(VERSION 3.26)
project(newGame)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")

set(CMAKE_CXX_STANDARD 20)

add_executable(newGame main.cpp
        enumRefs.cpp
        include/UTIL.h
        include/ACTORS.h
        include/DATA.h
        include/GRAPHICS.h
        include/UI.h
        include/COLLISION.h
        include/ANIMATION.h
        )

add_executable(EditorF fighterEditor.cpp
        enumRefs.cpp
        include/UTIL.h
        include/ACTORS.h
        include/DATA.h
        include/GRAPHICS.h
        include/UI.h
        include/COLLISION.h
        include/ANIMATION.h
)

set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML/lib/cmake/SFML")

target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)

find_package(SFML COMPONENTS system network window audio graphics CONFIG REQUIRED)
target_link_libraries(newGame PUBLIC sfml-system sfml-network sfml-audio sfml-window sfml-graphics)
target_link_libraries(EditorF PUBLIC sfml-system sfml-audio sfml-window sfml-graphics)
