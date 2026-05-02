#pragma once
#include <cstdint>

class Timer {
public:
    Timer();
    void setDelay(uint8_t value);
    void setSound(uint8_t value);
    uint8_t getDelay() const;
    uint8_t getSound() const;
    void update();
    bool isSoundBeep() const;

private:
    uint8_t soundTimer;
    uint8_t delayTimer;
};