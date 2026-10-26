#include "chip8.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

// ---------------------------------------------------------------------------
//  Construction
// ---------------------------------------------------------------------------

Chip8::Chip8()
    : memory(),
      display(),
      keyboard(),
      timer(),
      cpu(memory, display, keyboard, timer) {
    cpu.reset();
}

// ---------------------------------------------------------------------------
//  ROM loading
// ---------------------------------------------------------------------------

bool Chip8::loadROM(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open ROM: " << path << std::endl;
        return false;
    }

    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Failed to read ROM: " << path << std::endl;
        return false;
    }

    // Guard against ROMs that don't fit in memory.
    constexpr size_t MAX_ROM_SIZE = Memory::RAM_SIZE - Memory::ROM_START;
    if (buffer.size() > MAX_ROM_SIZE) {
        std::cerr << "ROM too large: " << buffer.size()
                  << " bytes (max " << MAX_ROM_SIZE << ")" << std::endl;
        return false;
    }

    memory.loadROM(buffer, Memory::ROM_START);
    cpu.reset();
    return true;
}

// ---------------------------------------------------------------------------
//  Delegation to subsystems (facade pattern)
// ---------------------------------------------------------------------------

void Chip8::cycle() {
    cpu.cycle();
}

void Chip8::keyDown(uint8_t key) {
    keyboard.keyDown(key);
}

void Chip8::keyUp(uint8_t key) {
    keyboard.keyUp(key);
}

void Chip8::updateTimers() {
    timer.update();
}

bool Chip8::shouldDraw() const {
    return display.shouldDraw();
}

void Chip8::drawn() {
    display.drawn();
}

// ---------------------------------------------------------------------------
//  Queries
// ---------------------------------------------------------------------------

const std::array<bool, Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT>&
Chip8::getFramebuffer() const {
    return display.getFramebuffer();
}

bool Chip8::isSoundBeeping() const {
    return timer.isSoundBeeping();
}