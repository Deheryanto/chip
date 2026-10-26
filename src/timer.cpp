#include "timer.h"

// ---------------------------------------------------------------------------
//  Construction
// ---------------------------------------------------------------------------

Timer::Timer() : delayTimer(0), soundTimer(0) {}

// ---------------------------------------------------------------------------
//  Setters / getters
// ---------------------------------------------------------------------------

void Timer::setDelay(uint8_t value) { delayTimer = value; }
void Timer::setSound(uint8_t value) { soundTimer = value; }

uint8_t Timer::getDelay() const { return delayTimer; }
uint8_t Timer::getSound() const { return soundTimer; }

// ---------------------------------------------------------------------------
//  60 Hz tick — decrements both timers (guarded against underflow)
// ---------------------------------------------------------------------------

void Timer::update() {
    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) --soundTimer;
}

// ---------------------------------------------------------------------------
//  Buzzer state
// ---------------------------------------------------------------------------

bool Timer::isSoundBeeping() const { return soundTimer > 0; }