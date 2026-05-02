#include <keyboard.h>
#include <chrono>
#include <thread>
#include <functional>

Keyboard::Keyboard() : keys {} {}

void Keyboard::keyDown(uint8_t key) { 
    if (key < 16) { 
        keys[key] = true;
    }
 }

void Keyboard::keyUp(uint8_t key) { 
    if (key < 16) {
        keys[key] = false;
    } 
}

bool Keyboard::isKeyPressed(uint8_t key) const { return key < 16 && keys[key]; }

uint8_t Keyboard::waitForKey() {
    for (uint8_t i = 0; i < 16; i++) {
        if (keys[i]) {
            return i; 
        }
    }
    return 0xFF;
}

void Keyboard::setWaitCallback(std::function<uint8_t()> callback) { waitcallback = callback; }