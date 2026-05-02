#pragma once
#include <array>
#include <vector>
#include <cstdint>

class Memory {
public: 
    Memory();
    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t value);
    void loadFont();
    void loadROM(const std::vector<uint8_t>& rom, uint16_t offset = 0x200);

private:
    std::array<uint8_t, 4096> ram;
    static const std::array<uint8_t, 80> fontset;
};