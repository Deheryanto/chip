#pragma once

// ---------------------------------------------------------------------------
//  memory.h — CHIP-8 system memory (4KB RAM + built-in fontset)
//
//  The CHIP-8 has 4096 bytes (0x000 .. 0xFFF) of addressable memory:
//      - 0x000 .. 0x1FF : Reserved for the interpreter / fontset (80 bytes)
//      - 0x200 .. 0xFFF : Program (ROM) space, loaded at 0x200
//
//  The fontset contains 16 hexadecimal sprites (0-F), each 5 bytes long,
//  stored at 0x000. The CPU's FX29 opcode reads them for digit rendering.
// ---------------------------------------------------------------------------

#include <array>
#include <cstdint>
#include <vector>

class Memory {
public:
    // CHIP-8 memory architecture constants.
    static constexpr size_t RAM_SIZE     = 4096;  // 4KB total RAM
    static constexpr size_t FONTSET_SIZE = 80;    // 16 sprites x 5 bytes
    static constexpr uint16_t ROM_START  = 0x200; // Where ROMs are loaded

    /**
     * @brief Constructs memory with all bytes zeroed, then loads the fontset
     *        into 0x000 .. 0x04F.
     */
    Memory();

    /**
     * @brief Reads a byte from memory.
     * @param addr Address (0x000 .. 0xFFF). Must be in range.
     * @return The byte stored at that address.
     *
     * @note Caller is responsible for keeping addr < RAM_SIZE. Reading
     *       out-of-bounds is undefined behavior.
     */
    uint8_t read(uint16_t addr) const;

    /**
     * @brief Writes a byte to memory.
     * @param addr  Address (0x000 .. 0xFFF). Must be in range.
     * @param value Byte to write.
     *
     * @note Caller is responsible for keeping addr < RAM_SIZE.
     */
    void write(uint16_t addr, uint8_t value);

    /**
     * @brief Copies the built-in 80-byte fontset into 0x000 .. 0x04F.
     *        Called automatically by the constructor.
     */
    void loadFont();

    /**
     * @brief Loads a ROM image into memory.
     * @param rom    The raw ROM bytes.
     * @param offset Destination address (default 0x200, per CHIP-8 spec).
     *
     * @note The ROM must fit within RAM_SIZE - offset (= 3584 bytes by
     *       default). Behavior is undefined if it does not fit.
     */
    void loadROM(const std::vector<uint8_t>& rom, uint16_t offset = ROM_START);

private:
    std::array<uint8_t, RAM_SIZE> ram;                  // 4KB system RAM
    static const std::array<uint8_t, FONTSET_SIZE> fontset; // Hex sprites 0-F
};