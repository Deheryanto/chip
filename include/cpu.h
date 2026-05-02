#pragma once
#include <array>
#include <cstdint>
#include <string>
#include "memory.h"
#include "display.h"
#include "keyboard.h"
#include "timer.h"

class CPU {
public:
    CPU(Memory& mem, Display& display, Keyboard& key, Timer& timer);
    void reset();
    void cycle();
    bool loadROM(const std::string& path);

private:
    Memory& memory;
    Display& display;
    Keyboard& keyboard;
    Timer& timer;

    std::array<uint8_t, 16> V; // Registers V0-VF
    uint16_t I; // Index Register
    uint16_t PC; // Program Counter
    std::array<uint16_t, 16> stack; 
    uint8_t SP; // Stack Pointer

    void execute(uint16_t opcode);
    void clear_screen(); // Clear Screen
    void op_00EE(); // Return from subroutine
    void op_1nnn(uint16_t nnn); // Jump to nnn
    void op_2nnn(uint16_t nnn); // Call subroutine at nnn
    void op_3xkk(uint8_t x, uint8_t kk);
    void op_4xkk(uint8_t x, uint8_t kk);
    void op_5xy0(uint8_t x, uint8_t y);
    void op_6xkk(uint8_t x, uint8_t kk);
    void op_7xkk(uint8_t x, uint8_t kk);
    void op_8xy0(uint8_t x, uint8_t y);
    void op_8xy1(uint8_t x, uint8_t y);
    void op_8xy2(uint8_t x, uint8_t y);
    void op_8xy3(uint8_t x, uint8_t y);
    void op_8xy4(uint8_t x, uint8_t y);
    void op_8xy5(uint8_t x, uint8_t y);
    void op_8xy6(uint8_t x, uint8_t y);
    void op_8xy7(uint8_t x, uint8_t y);
    void op_8xyE(uint8_t x, uint8_t y);
    void op_9xy0(uint8_t x, uint8_t y);
    void op_Annn(uint16_t nnn);
    void op_Bnnn(uint16_t nnn);
    void op_Cxkk(uint8_t x, uint8_t kk);
    void op_Dxyn(uint8_t x, uint8_t y, uint8_t n);
    void op_Ex9E(uint8_t x);
    void op_ExA1(uint8_t x);
    void op_Fx07(uint8_t x);
    void op_Fx0A(uint8_t x);
    void op_Fx15(uint8_t x);
    void op_Fx18(uint8_t x);
    void op_Fx1E(uint8_t x);
    void op_Fx29(uint8_t x);
    void op_Fx33(uint8_t x);
    void op_Fx55(uint8_t x);
    void op_Fx65(uint8_t x);
};