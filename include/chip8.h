#pragma once

// ---------------------------------------------------------------------------
//  chip8.h — Top-level facade for the CHIP-8 emulator
//
//  This class acts as the main "brain" that orchestrates all virtual
//  CHIP-8 hardware components:
//      - Memory   : 4KB RAM + fontset
//      - CPU      : Fetch, decode, execute opcodes
//      - Display  : 64x32 monochrome framebuffer
//      - Keyboard : 16-key hex input (0-F)
//      - Timer    : Delay timer & sound timer (60 Hz)
// ---------------------------------------------------------------------------

#include "cpu.h"
#include "display.h"
#include "keyboard.h"
#include "memory.h"
#include "timer.h"

#include <array>
#include <cstdint>
#include <string>

class Chip8 {
public:
    // CHIP-8 screen dimensions (64 x 32 monochrome pixels).
    static constexpr int DISPLAY_WIDTH  = 64;
    static constexpr int DISPLAY_HEIGHT = 32;

    /**
     * @brief Default constructor.
     * Performs initial setup: resets memory, loads the fontset,
     * clears the display, and resets all CPU registers to 0.
     */
    Chip8();

    /**
     * @brief Loads a ROM file (.ch8) into the emulator's memory.
     * @param path Path to the ROM file.
     * @return true on success, false if the file cannot be opened.
     */
    bool loadROM(const std::string& path);

    /**
     * @brief Executes one CPU cycle (fetch-decode-execute).
     * Typically called 500-700 times per second (CHIP-8 clock speed).
     */
    void cycle();

    /**
     * @brief Marks a physical keyboard key as "pressed".
     * @param key Key index (0x0 - 0xF).
     */
    void keyDown(uint8_t key);

    /**
     * @brief Marks a physical keyboard key as "released".
     * @param key Key index (0x0 - 0xF).
     */
    void keyUp(uint8_t key);

    /**
     * @brief Decrements the delay timer and sound timer.
     * Must be called exactly 60 times per second.
     */
    void updateTimers();

    /**
     * @brief Checks whether the framebuffer has changed and needs redrawing.
     * Uses a "dirty" flag to avoid redundant rendering.
     */
    bool shouldDraw() const;

    /**
     * @brief Resets the dirty flag after a frame has been rendered.
     */
    void drawn();

    /**
     * @brief Returns a reference to the framebuffer (no copy).
     * @return Reference to a 64x32 boolean array (true = pixel on).
     */
    const std::array<bool, DISPLAY_WIDTH * DISPLAY_HEIGHT>& getFramebuffer() const;

    /**
     * @brief Checks whether the sound timer is still active (buzzer on).
     */
    bool isSoundBeeping() const;

private:
    Memory   memory;    // 4KB RAM
    Display  display;   // 64x32 framebuffer
    Keyboard keyboard;  // State of 16 hex keys
    Timer    timer;     // Delay & sound timers
    CPU      cpu;       // Opcode processing unit
};