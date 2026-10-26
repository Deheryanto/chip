#pragma once

// ---------------------------------------------------------------------------
//  cpu.h — CHIP-8 CPU (instruction interpreter)
//
//  The CPU fetches 16-bit opcodes from Memory, decodes them, and executes
//  the corresponding operation. It holds all architectural registers:
//
//      - V[0..F] : 16 general-purpose 8-bit registers
//      - I       : 16-bit index register (used for memory addressing)
//      - PC      : 16-bit program counter
//      - stack   : 16-level call stack (for 2NNN / 00EE)
//      - SP      : 8-bit stack pointer
//
//  The CPU does NOT own any hardware component; it receives references
//  to Memory, Display, Keyboard, and Timer via the constructor.
// ---------------------------------------------------------------------------

#include "display.h"
#include "keyboard.h"
#include "memory.h"
#include "timer.h"

#include <array>
#include <cstdint>
#include <string>

class CPU {
public:
    // CHIP-8 architecture constants.
    static constexpr size_t NUM_REGISTERS = 16;  // V0 .. VF
    static constexpr size_t STACK_DEPTH   = 16;  // 16-level call stack

    /**
     * @brief Constructs the CPU with references to the other components.
     * @param mem     Reference to system memory (4KB RAM).
     * @param display Reference to the 64x32 display.
     * @param key     Reference to the hex keypad.
     * @param timer   Reference to the delay/sound timers.
     */
    CPU(Memory& mem, Display& disp, Keyboard& key, Timer& timer);

    /**
     * @brief Resets all CPU registers (V, I, PC, SP, stack) to their
     *        initial state. Does NOT clear memory or display.
     */
    void reset();

    /**
     * @brief Executes one full instruction cycle:
     *        1. Fetch 16-bit opcode at PC
     *        2. Increment PC by 2
     *        3. Decode & execute the opcode
     */
    void cycle();

    /**
     * @brief Loads a ROM into memory (delegates to Memory).
     * @param path Path to the .ch8 ROM file.
     * @return true on success, false otherwise.
     *
     * @note Loading a ROM is arguably a Memory responsibility; this
     *       method exists as a convenience wrapper for Chip8.
     */
    bool loadROM(const std::string& path);

private:
    // --- Hardware dependencies (not owned) ---
    Memory&   memory;
    Display&  display;
    Keyboard& keyboard;
    Timer&    timer;

    // --- Architectural registers ---
    std::array<uint8_t,  NUM_REGISTERS> V;      // V0 .. VF
    uint16_t                            I;      // Index register
    uint16_t                            PC;     // Program counter
    std::array<uint16_t, STACK_DEPTH>   stack;  // Call stack
    uint8_t                             SP;     // Stack pointer

    // --- Instruction dispatch ---
    /**
     * @brief Decodes and dispatches a 16-bit opcode to the correct
     *        op_XXXX() handler based on the opcode's high nibble.
     * @param opcode The 16-bit opcode to execute.
     */
    void execute(uint16_t opcode);

    // --- Opcode handlers ---------------------------------------------------
    // Naming convention: op_XYZZ where X, Y = register nibbles and
    // ZZ = 8-bit immediate / low nibble, matching the CHIP-8 spec.

    void clear_screen();                       // 00E0 — Clear display
    void op_00EE();                            // 00EE — Return from subroutine
    void op_1nnn(uint16_t nnn);                // 1NNN — Jump to address NNN
    void op_2nnn(uint16_t nnn);                // 2NNN — Call subroutine at NNN
    void op_3xkk(uint8_t x, uint8_t kk);       // 3XKK — Skip if VX == KK
    void op_4xkk(uint8_t x, uint8_t kk);       // 4XKK — Skip if VX != KK
    void op_5xy0(uint8_t x, uint8_t y);        // 5XY0 — Skip if VX == VY
    void op_6xkk(uint8_t x, uint8_t kk);       // 6XKK — VX = KK
    void op_7xkk(uint8_t x, uint8_t kk);       // 7XKK — VX += KK
    void op_8xy0(uint8_t x, uint8_t y);        // 8XY0 — VX = VY
    void op_8xy1(uint8_t x, uint8_t y);        // 8XY1 — VX |= VY
    void op_8xy2(uint8_t x, uint8_t y);        // 8XY2 — VX &= VY
    void op_8xy3(uint8_t x, uint8_t y);        // 8XY3 — VX ^= VY
    void op_8xy4(uint8_t x, uint8_t y);        // 8XY4 — VX += VY (with carry)
    void op_8xy5(uint8_t x, uint8_t y);        // 8XY5 — VX -= VY (with borrow)
    void op_8xy6(uint8_t x, uint8_t y);        // 8XY6 — VX >>= 1
    void op_8xy7(uint8_t x, uint8_t y);        // 8XY7 — VX = VY - VX
    void op_8xyE(uint8_t x, uint8_t y);        // 8XYE — VX <<= 1
    void op_9xy0(uint8_t x, uint8_t y);        // 9XY0 — Skip if VX != VY
    void op_Annn(uint16_t nnn);                // ANNN — I = NNN
    void op_Bnnn(uint16_t nnn);                // BNNN — Jump to NNN + V0
    void op_Cxkk(uint8_t x, uint8_t kk);       // CXKK — VX = rand() & KK
    void op_Dxyn(uint8_t x, uint8_t y, uint8_t n); // DXYN — Draw sprite
    void op_Ex9E(uint8_t x);                   // EX9E — Skip if key VX pressed
    void op_ExA1(uint8_t x);                   // EXA1 — Skip if key VX not pressed
    void op_Fx07(uint8_t x);                   // FX07 — VX = delay_timer
    void op_Fx0A(uint8_t x);                   // FX0A — VX = wait for key
    void op_Fx15(uint8_t x);                   // FX15 — delay_timer = VX
    void op_Fx18(uint8_t x);                   // FX18 — sound_timer = VX
    void op_Fx1E(uint8_t x);                   // FX1E — I += VX
    void op_Fx29(uint8_t x);                   // FX29 — I = fontset[VX]
    void op_Fx33(uint8_t x);                   // FX33 — BCD of VX into I..I+2
    void op_Fx55(uint8_t x);                   // FX55 — Store V0..VX in memory
    void op_Fx65(uint8_t x);                   // FX65 — Load V0..VX from memory
};