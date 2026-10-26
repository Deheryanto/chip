#include "memory.h"

#include <algorithm>   // std::copy
#include <vector>

// ---------------------------------------------------------------------------
//  Built-in fontset — 16 hex digits (0-F), 5 bytes each
// ---------------------------------------------------------------------------

const std::array<uint8_t, Memory::FONTSET_SIZE> Memory::fontset = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// ---------------------------------------------------------------------------
//  Construction
// ---------------------------------------------------------------------------

Memory::Memory() : ram{} {
    loadFont();   // Copy fontset into 0x000..0x04F — REQUIRED for FX29/DXYN
}

// ---------------------------------------------------------------------------
//  Byte-level access
// ---------------------------------------------------------------------------

uint8_t Memory::read(uint16_t addr) const {
    // Defensive: return 0 for out-of-bounds (prevents crash if I+row overflows).
    return (addr < ram.size()) ? ram[addr] : 0;
}

void Memory::write(uint16_t addr, uint8_t value) {
    if (addr < ram.size()) {
        ram[addr] = value;
    }
}

// ---------------------------------------------------------------------------
//  Fontset
// ---------------------------------------------------------------------------

void Memory::loadFont() {
    std::copy(fontset.begin(), fontset.end(), ram.begin());
}

// ---------------------------------------------------------------------------
//  ROM loading
//
//  Caller is responsible for ensuring the ROM fits within
//  (RAM_SIZE - offset) bytes (see Chip8::loadROM).
// ---------------------------------------------------------------------------

void Memory::loadROM(const std::vector<uint8_t>& rom, uint16_t offset) {
    const size_t maxBytes = ram.size() - offset;
    const size_t toCopy   = std::min(rom.size(), maxBytes);
    std::copy(rom.begin(), rom.begin() + toCopy, ram.begin() + offset);
}