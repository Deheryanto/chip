#include "keyboard.h"

// ---------------------------------------------------------------------------
//  Construction
// ---------------------------------------------------------------------------

Keyboard::Keyboard() : keys{} {}

// ---------------------------------------------------------------------------
//  Key state manipulation
// ---------------------------------------------------------------------------

void Keyboard::keyDown(uint8_t key) {
    if (key < NUM_KEYS) {
        keys[key] = true;
    }
}

void Keyboard::keyUp(uint8_t key) {
    if (key < NUM_KEYS) {
        keys[key] = false;
    }
}

bool Keyboard::isKeyPressed(uint8_t key) const {
    return key < NUM_KEYS && keys[key];
}

// ---------------------------------------------------------------------------
//  FX0A support — non-blocking poll
//
//  Returns the index of any pressed key, or 0xFF if none is pressed.
//  The CPU's FX0A handler must check for 0xFF and re-execute the opcode
//  (PC -= 2) on the next cycle if no key was available.
// ---------------------------------------------------------------------------

uint8_t Keyboard::waitForKey() {
    for (uint8_t i = 0; i < NUM_KEYS; ++i) {
        if (keys[i]) return i;
    }
    return 0xFF;
}

// ---------------------------------------------------------------------------
//  Callback (currently unused with Design B — kept for future use)
// ---------------------------------------------------------------------------

void Keyboard::setWaitCallback(std::function<uint8_t()> callback) {
    waitCallback = std::move(callback);
}