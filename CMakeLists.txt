cmake_minimum_required(VERSION 3.10)
project(Chip8Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(SDL2 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

add_executable(chip8
    src/main.cpp
    src/memory.cpp
    src/cpu.cpp
    src/display.cpp
    src/keyboard.cpp
    src/timer.cpp
    src/chip8.cpp
)

target_include_directories(chip8 PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SDL2_TTF_INCLUDE_DIRS} 
)

target_link_libraries(chip8 PRIVATE 
    SDL2::SDL2 
    ${SDL2_TTF_LIBRARIES} 
)

target_compile_options(chip8 PRIVATE ${SDL2_TTF_CFLAGS_OTHER})
target_link_directories(chip8 PRIVATE ${SDL2_TTF_LIBRARY_DIRS})