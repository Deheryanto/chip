#pragma once 
#include <cstdint>
#include <array>
#include <functional>


class Keyboard {
public:
    Keyboard();
    void keyDown(uint8_t key);
    void keyUp(uint8_t key);
    bool isKeyPressed(uint8_t) const;
    uint8_t waitForKey();
    void setWaitCallback(std::function<uint8_t()> callback);

private:
    std::array<bool, 16> keys;
    std::function<uint8_t()> waitcallback;
};