#include <timer.h>

Timer::Timer() : delayTimer(0), soundTimer(0) {}

void Timer::setDelay(uint8_t value) { delayTimer = value; }
void Timer::setSound(uint8_t value) { soundTimer = value; }

uint8_t Timer::getDelay() const { return delayTimer; }
uint8_t Timer::getSound() const { return soundTimer; }

void Timer::update() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

bool Timer::isSoundBeep() const { return soundTimer > 0; }