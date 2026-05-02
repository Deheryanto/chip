#pragma once
#include "memory.h"
#include "cpu.h"
#include "display.h"
#include "keyboard.h"
#include "timer.h"
#include <vector>
#include <string>

class Chip8 {
public:
    Chip8();
    bool loadROM(const std::string& path);
    void cycle();
    void keyDown(uint8_t key);
    void keyUp(uint8_t key);
    void updateTimers();
    bool shouldDraw() const;
    void drawn();
    const std::array<bool, 64*32>& getFramebuffer() const;
    bool isSoundBeeping() const;

private:
    Memory memory;
    Display display;
    Keyboard keyboard;
    Timer timer;
    CPU cpu;
};