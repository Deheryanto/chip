#pragma once

// ---------------------------------------------------------------------------
//  keyboard.h — CHIP-8 hex keypad (16 keys, 0-F)
//
//  The CHIP-8 keypad has 16 keys labeled 0-F (hex). This class tracks
//  which keys are currently pressed. Opcodes EX9E / EXA1 query key state
//  synchronously, while FX0A must "wait" for a key press — which requires
//  access to the host event loop. To keep this class decoupled from the
//  host (SDL/GLFW/terminal), FX0A support is provided via a callback.
// ---------------------------------------------------------------------------

#include <array>
#include <cstdint>
#include <functional>

class Keyboard {
public:
    // CHIP-8 keypad size: 16 keys labeled 0x0 .. 0xF.
    static constexpr size_t NUM_KEYS = 16;

    /**
     * @brief Constructs the keyboard with all keys in the "released" state.
     */
    Keyboard();

    /**
     * @brief Marks a key as pressed.
     * @param key Key index (0x0 .. 0xF). Values >= 16 are ignored.
     */
    void keyDown(uint8_t key);

    /**
     * @brief Marks a key as released.
     * @param key Key index (0x0 .. 0xF). Values >= 16 are ignored.
     */
    void keyUp(uint8_t key);

    /**
     * @brief Checks whether a given key is currently pressed.
     * @param key Key index (0x0 .. 0xF).
     * @return true if pressed, false otherwise.
     */
    bool isKeyPressed(uint8_t key) const;

    /**
     * @brief Blocks until a key is pressed, then returns its index.
     *
     * How blocking is achieved depends on the registered wait callback.
     * If no callback is set, this may spin-wait — see setWaitCallback().
     *
     * @return The index (0x0 .. 0xF) of the key that was pressed.
     */
    uint8_t waitForKey();

    /**
     * @brief Registers a callback used to poll/wait for key input.
     *
     * The callback should block until the host has delivered a key press,
     * then return the key index (0x0 .. 0xF). This is how the CPU's FX0A
     * opcode integrates with the host event loop (e.g. SDL_WaitEvent).
     *
     * Pass a default-constructed std::function to clear the callback.
     *
     * @param callback Function returning the pressed key index.
     */
    void setWaitCallback(std::function<uint8_t()> callback);

private:
    std::array<bool, NUM_KEYS> keys;           // Key states (true = pressed)
    std::function<uint8_t()>   waitCallback;   // Host hook for FX0A
};