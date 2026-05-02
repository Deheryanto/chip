cmake_minimum_required(VERSION 3.10)
project(Chip8Emulator)

set(CMAKE_CXX_STANDARD 17)
find_package(SDL2 REQUIRED)

add_executable(chip8 src/main.cpp src/memory.cpp src/cpu.cpp src/display.cpp src/keyboard.cpp src/timer.cpp src/chip8.cpp)
target_include_directories(chip8 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(chip8 SDL2::SDL2)
