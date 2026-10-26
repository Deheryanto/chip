#include "display.h"

// ---------------------------------------------------------------------------
//  Construction
// ---------------------------------------------------------------------------

Display::Display() : frameBuffer{}, drawRequested(true) {
    // frameBuffer{} zero-initializes all pixels to false; nothing else needed.
}

// ---------------------------------------------------------------------------
//  Pixel buffer manipulation
// ---------------------------------------------------------------------------

void Display::clear() {
    frameBuffer.fill(false);
    drawRequested = true;
}

void Display::setPixel(int x, int y, bool state) {
    // Wrap around screen edges (CHIP-8 spec).
    int finalX = x % WIDTH;
    int finalY = y % HEIGHT;

    if (finalX < 0) finalX += WIDTH;
    if (finalY < 0) finalY += HEIGHT;

    frameBuffer[finalY * WIDTH + finalX] = state;
}

bool Display::getPixel(int x, int y) const {
    // Out-of-bounds → treat as "off" (CHIP-8 spec tolerant).
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return false;
    }
    return frameBuffer[y * WIDTH + x];
}

// ---------------------------------------------------------------------------
//  Framebuffer access
// ---------------------------------------------------------------------------

const std::array<bool, Display::WIDTH * Display::HEIGHT>&
Display::getFramebuffer() const {
    return frameBuffer;
}

// ---------------------------------------------------------------------------
//  Dirty flag
// ---------------------------------------------------------------------------

void Display::requestDraw() { drawRequested = true; }
bool Display::shouldDraw() const { return drawRequested; }
void Display::drawn() { drawRequested = false; }