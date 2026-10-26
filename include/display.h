#pragma once

// ---------------------------------------------------------------------------
//  display.h — CHIP-8 monochrome display (64 x 32 pixels)
//
//  The display holds a 2048-bit monochrome framebuffer (64 * 32 = 2048).
//  Each pixel is either ON (true) or OFF (false). A "dirty" flag is used
//  to avoid re-rendering when nothing has changed, which is important
//  because the CPU runs at ~500-700 Hz while the screen refreshes at
//  ~60 Hz.
//
//  Note: CHIP-8 sprites wrap around the edges of the screen. This class
//  is responsible only for storing/querying pixel state — the wrapping
//  logic belongs in the CPU's DXYN handler.
// ---------------------------------------------------------------------------

#include <array>
#include <cstdint>

class Display {
public:
    // CHIP-8 screen dimensions (64 x 32 monochrome pixels).
    static constexpr int WIDTH  = 64;
    static constexpr int HEIGHT = 32;

    /**
     * @brief Constructs the display with all pixels OFF.
     * Sets the draw-request flag so the first frame is rendered.
     */
    Display();

    /**
     * @brief Clears the entire framebuffer (all pixels OFF).
     * Should also request a redraw so the cleared state is rendered.
     */
    void clear();

    /**
     * @brief Sets a single pixel on or off.
     * @param x     X coordinate (0 .. WIDTH-1).
     * @param y     Y coordinate (0 .. HEIGHT-1).
     * @param state true = pixel ON, false = pixel OFF.
     */
    void setPixel(int x, int y, bool state);

    /**
     * @brief Reads a single pixel's state.
     * @param x X coordinate (0 .. WIDTH-1).
     * @param y Y coordinate (0 .. HEIGHT-1).
     * @return true if the pixel is ON, false otherwise.
     */
    bool getPixel(int x, int y) const;

    /**
     * @brief Returns a const reference to the framebuffer (no copy).
     * The array is laid out row-major: index = y * WIDTH + x.
     */
    const std::array<bool, WIDTH * HEIGHT>& getFramebuffer() const;

    /**
     * @brief Marks the framebuffer as "needs redraw".
     * Typically called after any pixel mutation.
     */
    void requestDraw();

    /**
     * @brief Returns true if the framebuffer has changed since the last draw.
     */
    bool shouldDraw() const;

    /**
     * @brief Clears the draw-request flag after a frame has been rendered.
     */
    void drawn();

private:
    std::array<bool, WIDTH * HEIGHT> frameBuffer;   // Row-major pixel buffer
    bool drawRequested;                             // "Dirty" flag for rendering
};