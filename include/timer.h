#pragma once

// ---------------------------------------------------------------------------
//  timer.h — CHIP-8 delay & sound timers
//
//  The CHIP-8 has two 8-bit down-counters:
//
//      - delayTimer : read/written by opcodes FX07 / FX15; when it reaches
//                     zero, it stops (no side effect).
//      - soundTimer : written by opcode FX18; while > 0, the buzzer sounds.
//
//  Both tick at 60 Hz — i.e., update() must be called exactly 60 times
//  per second. A value of N means "N / 60 seconds remaining".
// ---------------------------------------------------------------------------

#include <cstdint>

class Timer {
public:
    /**
     * @brief Constructs the timer with both counters at zero.
     */
    Timer();

    /**
     * @brief Sets the delay timer.
     * @param value New value (0 .. 255). 0 stops the timer.
     */
    void setDelay(uint8_t value);

    /**
     * @brief Sets the sound timer.
     * @param value New value (0 .. 255). 0 silences the buzzer.
     */
    void setSound(uint8_t value);

    /**
     * @brief Returns the current delay timer value.
     */
    uint8_t getDelay() const;

    /**
     * @brief Returns the current sound timer value.
     */
    uint8_t getSound() const;

    /**
     * @brief Decrements both timers by 1 (if greater than zero).
     *        Must be called exactly 60 times per second (60 Hz).
     *
     * @note Timers must NOT underflow: check `> 0` before decrementing,
     *       otherwise uint8_t wraps from 0 to 255.
     */
    void update();

    /**
     * @brief Returns true while the sound timer is > 0 (buzzer active).
     */
    bool isSoundBeeping() const;

private:
    uint8_t delayTimer;   // Generic delay counter (0 = expired)
    uint8_t soundTimer;   // Buzzer duration remaining (0 = silent)
};