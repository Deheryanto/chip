#include "chip8.h"
#include <fstream>
#include <iostream>
#include <vector>

Chip8::Chip8()
    : memory(), display(), keyboard(), timer(), cpu(memory, display, keyboard, timer) {
    cpu.reset();
}

bool Chip8::loadROM(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open ROM: " << path << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Failed to read ROM: " << path << std::endl;
        return false;
    }

    memory.loadROM(buffer, 0x200);
    cpu.reset();
    return true;
}

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

const std::array<bool, 64 * 32>& Chip8::getFramebuffer() const {
    return display.getFrameBuffer();
}

bool Chip8::isSoundBeeping() const {
    return timer.isSoundBeep();
}