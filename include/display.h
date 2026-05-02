#pragma once
#include <array>
#include <cstdint>
#include <functional>

class Display {
public:
    static constexpr int WIDTH = 64;
    static constexpr int HEIGHT = 32;

    Display();
    void clear();
    void setPixel(int x, int y, bool state);
    bool getPixel(int x, int y) const;
    const std::array<bool,  WIDTH*HEIGHT>& getFrameBuffer() const;
    void requestDraw();
    bool shouldDraw() const;
    void drawn();

private:
    std::array<bool, WIDTH*HEIGHT> frameBuffer;
    bool drawRequested;
};