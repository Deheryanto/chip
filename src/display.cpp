#include "display.h"

Display::Display() : frameBuffer{}, drawRequested(true) {
    frameBuffer.fill(false);
    clear();
}

void Display::clear() {
    frameBuffer.fill(false);
    drawRequested = true;
}

void Display::setPixel(int x, int y, bool state) {
    int finalX = x % WIDTH;
    int finalY = y % HEIGHT;

    if (finalX < 0) finalX += WIDTH;
    if (finalY < 0) finalY += HEIGHT;

    frameBuffer[finalY * WIDTH + finalX] = state;
}

bool Display::getPixel(int x, int y) const {
    if (x >= 0 && x <= WIDTH && y >= 0 && y <= HEIGHT) {
        return frameBuffer[y * WIDTH + x] != 0;
    }
    return false;
}

const std::array<bool,  Display::WIDTH*Display::HEIGHT>& Display::getFrameBuffer() const {
    return frameBuffer;
}

void Display::requestDraw() { drawRequested = true; }
bool Display::shouldDraw() const { return drawRequested; }
void Display::drawn() { drawRequested = false; }