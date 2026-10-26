#include "cpu.h"

#include <random>

// ---------------------------------------------------------------------------
//  Construction / reset
// ---------------------------------------------------------------------------

CPU::CPU(Memory& mem, Display& disp, Keyboard& key, Timer& t)
    : memory(mem), display(disp), keyboard(key), timer(t) {
    reset();
}

void CPU::reset() {
    V.fill(0);
    I  = 0;
    PC = 0x200;
    stack.fill(0);
    SP = 0;
    // NOTE: fontset loading is Memory's responsibility (done in Memory()).
    // Calling memory.loadFont() here was redundant and violated SRP.
}

// ---------------------------------------------------------------------------
//  Fetch-decode-execute
// ---------------------------------------------------------------------------

void CPU::cycle() {
    uint16_t opcode = (memory.read(PC) << 8) | memory.read(PC + 1);
    PC += 2;
    execute(opcode);
}

void CPU::execute(uint16_t opcode) {
    const uint16_t nnn = opcode & 0x0FFF;
    const uint8_t  n   = opcode & 0x000F;
    const uint8_t  x   = (opcode & 0x0F00) >> 8;
    const uint8_t  y   = (opcode & 0x00F0) >> 4;
    const uint8_t  kk  = opcode & 0x00FF;

    switch ((opcode & 0xF000) >> 12) {
    case 0x0:
        if (opcode == 0x00E0)      clear_screen();
        else if (opcode == 0x00EE) op_00EE();
        break;
    case 0x1: op_1nnn(nnn);       break;
    case 0x2: op_2nnn(nnn);       break;
    case 0x3: op_3xkk(x, kk);     break;
    case 0x4: op_4xkk(x, kk);     break;
    case 0x5: op_5xy0(x, y);      break;
    case 0x6: op_6xkk(x, kk);     break;
    case 0x7: op_7xkk(x, kk);     break;
    case 0x8:
        switch (opcode & 0x000F) {
        case 0x0: op_8xy0(x, y); break;
        case 0x1: op_8xy1(x, y); break;
        case 0x2: op_8xy2(x, y); break;
        case 0x3: op_8xy3(x, y); break;
        case 0x4: op_8xy4(x, y); break;
        case 0x5: op_8xy5(x, y); break;
        case 0x6: op_8xy6(x, y); break;
        case 0x7: op_8xy7(x, y); break;
        case 0xE: op_8xyE(x, y); break;
        }
        break;
    case 0x9: op_9xy0(x, y);      break;
    case 0xA: op_Annn(nnn);       break;
    case 0xB: op_Bnnn(nnn);       break;
    case 0xC: op_Cxkk(x, kk);     break;
    case 0xD: op_Dxyn(x, y, n);   break;
    case 0xE:
        if      ((opcode & 0x00FF) == 0x009E) op_Ex9E(x);
        else if ((opcode & 0x00FF) == 0x00A1) op_ExA1(x);
        break;
    case 0xF:
        switch (opcode & 0x00FF) {
        case 0x0007: op_Fx07(x); break;
        case 0x000A: op_Fx0A(x); break;
        case 0x0015: op_Fx15(x); break;
        case 0x0018: op_Fx18(x); break;
        case 0x001E: op_Fx1E(x); break;
        case 0x0029: op_Fx29(x); break;
        case 0x0033: op_Fx33(x); break;
        case 0x0055: op_Fx55(x); break;
        case 0x0065: op_Fx65(x); break;
        }
        break;
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
//  00E0 - Clear screen
// ---------------------------------------------------------------------------

void CPU::clear_screen() {
    display.clear();
    display.requestDraw();
}

// ---------------------------------------------------------------------------
//  00EE - Return from subroutine
// ---------------------------------------------------------------------------

void CPU::op_00EE() {
    if (SP > 0) {
        --SP;
        PC = stack[SP];
    }
}

// ---------------------------------------------------------------------------
//  1NNN - Jump to address NNN
// ---------------------------------------------------------------------------

void CPU::op_1nnn(uint16_t nnn) { PC = nnn; }

// ---------------------------------------------------------------------------
//  2NNN - Call subroutine at NNN
// ---------------------------------------------------------------------------

void CPU::op_2nnn(uint16_t nnn) {
    if (SP < 16) {
        stack[SP] = PC;
        ++SP;
        PC = nnn;
    }
}

// ---------------------------------------------------------------------------
//  3XKK - Skip next instruction if VX == KK
// ---------------------------------------------------------------------------

void CPU::op_3xkk(uint8_t x, uint8_t kk) {
    if (V[x] == kk) PC += 2;
}

// ---------------------------------------------------------------------------
//  4XKK - Skip next instruction if VX != KK
// ---------------------------------------------------------------------------

void CPU::op_4xkk(uint8_t x, uint8_t kk) {
    if (V[x] != kk) PC += 2;
}

// ---------------------------------------------------------------------------
//  5XY0 - Skip next instruction if VX == VY
// ---------------------------------------------------------------------------

void CPU::op_5xy0(uint8_t x, uint8_t y) {
    if (V[x] == V[y]) PC += 2;
}

// ---------------------------------------------------------------------------
//  6XKK - VX = KK
// ---------------------------------------------------------------------------

void CPU::op_6xkk(uint8_t x, uint8_t kk) { V[x] = kk; }

// ---------------------------------------------------------------------------
//  7XKK - VX += KK (no carry flag)
// ---------------------------------------------------------------------------

void CPU::op_7xkk(uint8_t x, uint8_t kk) { V[x] += kk; }

// ---------------------------------------------------------------------------
//  8XY0 - VX = VY
// ---------------------------------------------------------------------------

void CPU::op_8xy0(uint8_t x, uint8_t y) { V[x] = V[y]; }

// ---------------------------------------------------------------------------
//  8XY1 - VX |= VY
// ---------------------------------------------------------------------------

void CPU::op_8xy1(uint8_t x, uint8_t y) { V[x] |= V[y]; }

// ---------------------------------------------------------------------------
//  8XY2 - VX &= VY
// ---------------------------------------------------------------------------

void CPU::op_8xy2(uint8_t x, uint8_t y) { V[x] &= V[y]; }

// ---------------------------------------------------------------------------
//  8XY3 - VX ^= VY
// ---------------------------------------------------------------------------

void CPU::op_8xy3(uint8_t x, uint8_t y) { V[x] ^= V[y]; }

// ---------------------------------------------------------------------------
//  8XY4 - VX += VY, VF = carry
// ---------------------------------------------------------------------------

void CPU::op_8xy4(uint8_t x, uint8_t y) {
    const uint16_t sum = static_cast<uint16_t>(V[x]) + V[y];
    V[0xF] = (sum > 0xFF) ? 1 : 0;
    V[x]   = static_cast<uint8_t>(sum & 0xFF);
}

// ---------------------------------------------------------------------------
//  8XY5 - VX -= VY, VF = NOT borrow
// ---------------------------------------------------------------------------

void CPU::op_8xy5(uint8_t x, uint8_t y) {
    V[0xF] = (V[x] >= V[y]) ? 1 : 0;
    V[x]   = static_cast<uint8_t>(V[x] - V[y]);
}

// ---------------------------------------------------------------------------
//  8XY6 - VX >>= 1, VF = LSB
//
//  Using the MODERN (CHIP-48/SCHIP) behavior: shifts VX in place.
//  The 'y' register is intentionally unused. See cpu.h for a note.
// ---------------------------------------------------------------------------

void CPU::op_8xy6(uint8_t x, [[maybe_unused]] uint8_t y) {
    V[0xF] = V[x] & 0x1;
    V[x] >>= 1;
}

// ---------------------------------------------------------------------------
//  8XY7 - VX = VY - VX, VF = NOT borrow
// ---------------------------------------------------------------------------

void CPU::op_8xy7(uint8_t x, uint8_t y) {
    V[0xF] = (V[y] >= V[x]) ? 1 : 0;
    V[x]   = static_cast<uint8_t>(V[y] - V[x]);
}

// ---------------------------------------------------------------------------
//  8XYE - VX <<= 1, VF = MSB
//
//  Using the MODERN (CHIP-48/SCHIP) behavior: shifts VX in place.
// ---------------------------------------------------------------------------

void CPU::op_8xyE(uint8_t x, [[maybe_unused]] uint8_t y) {
    V[0xF] = (V[x] & 0x80) >> 7;
    V[x] <<= 1;
}

// ---------------------------------------------------------------------------
//  9XY0 - Skip next instruction if VX != VY
// ---------------------------------------------------------------------------

void CPU::op_9xy0(uint8_t x, uint8_t y) {
    if (V[x] != V[y]) PC += 2;
}

// ---------------------------------------------------------------------------
//  ANNN - I = NNN
// ---------------------------------------------------------------------------

void CPU::op_Annn(uint16_t nnn) { I = nnn; }

// ---------------------------------------------------------------------------
//  BNNN - Jump to NNN + V0
// ---------------------------------------------------------------------------

void CPU::op_Bnnn(uint16_t nnn) { PC = static_cast<uint16_t>(nnn + V[0x0]); }

// ---------------------------------------------------------------------------
//  CXKK - VX = rand() & KK
// ---------------------------------------------------------------------------

void CPU::op_Cxkk(uint8_t x, uint8_t kk) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    // std::uniform_int_distribution<uint8_t> is UB per the standard;
    // use <int> and cast.
    static std::uniform_int_distribution<int> dist(0, 255);
    V[x] = static_cast<uint8_t>(dist(gen)) & kk;
}

// ---------------------------------------------------------------------------
//  DXYN - Draw sprite at (VX, VY), N bytes tall
//
//  Original CHIP-8 spec says sprites wrap around the screen edges.
//  We use wrap (% WIDTH/HEIGHT) for maximum compatibility.
// ---------------------------------------------------------------------------

void CPU::op_Dxyn(uint8_t x, uint8_t y, uint8_t n) {
    const uint8_t x0 = V[x] % Display::WIDTH;
    const uint8_t y0 = V[y] % Display::HEIGHT;
    V[0xF] = 0;

    for (uint8_t row = 0; row < n; ++row) {
        const uint8_t sprite_byte = memory.read(static_cast<uint16_t>(I + row));
        const uint8_t py = (y0 + row) % Display::HEIGHT;

        for (uint8_t col = 0; col < 8; ++col) {
            if ((sprite_byte & (0x80 >> col)) == 0) continue;

            const uint8_t px = (x0 + col) % Display::WIDTH;
            const bool current_pixel = display.getPixel(px, py);

            if (current_pixel) V[0xF] = 1;            // collision
            display.setPixel(px, py, !current_pixel); // XOR
        }
    }
    display.requestDraw();
}

// ---------------------------------------------------------------------------
//  EX9E - Skip next instruction if key VX is pressed
// ---------------------------------------------------------------------------

void CPU::op_Ex9E(uint8_t x) {
    if (keyboard.isKeyPressed(V[x])) PC += 2;
}

// ---------------------------------------------------------------------------
//  EXA1 - Skip next instruction if key VX is NOT pressed
// ---------------------------------------------------------------------------

void CPU::op_ExA1(uint8_t x) {
    if (!keyboard.isKeyPressed(V[x])) PC += 2;
}

// ---------------------------------------------------------------------------
//  FX07 - VX = delay_timer
// ---------------------------------------------------------------------------

void CPU::op_Fx07(uint8_t x) { V[x] = timer.getDelay(); }

// ---------------------------------------------------------------------------
//  FX0A - Wait for a key press, store it in VX
//
//  keyboard.waitForKey() is BLOCKING: it must return a valid key index
//  (0x0..0xF) and never 0xFF. If you later switch to a non-blocking
//  implementation, restore `PC -= 2` here.
// ---------------------------------------------------------------------------

void CPU::op_Fx0A(uint8_t x) {
    uint8_t key = keyboard.waitForKey();
    if (key != 0xFF) {
        V[x] = key;
    } else {
        PC -= 2;   // re-execute next cycle
    }
}

// ---------------------------------------------------------------------------
//  FX15 - delay_timer = VX
// ---------------------------------------------------------------------------

void CPU::op_Fx15(uint8_t x) { timer.setDelay(V[x]); }

// ---------------------------------------------------------------------------
//  FX18 - sound_timer = VX
// ---------------------------------------------------------------------------

void CPU::op_Fx18(uint8_t x) {
    timer.setSound(V[x]);
}

// ---------------------------------------------------------------------------
//  FX1E - I += VX
// ---------------------------------------------------------------------------

void CPU::op_Fx1E(uint8_t x) { I = static_cast<uint16_t>(I + V[x]); }

// ---------------------------------------------------------------------------
//  FX29 - I = fontset address of digit VX (each digit is 5 bytes)
// ---------------------------------------------------------------------------

void CPU::op_Fx29(uint8_t x) {
    I = static_cast<uint16_t>(V[x] * 5);
}

// ---------------------------------------------------------------------------
//  FX33 - Store BCD of VX at I, I+1, I+2
// ---------------------------------------------------------------------------

void CPU::op_Fx33(uint8_t x) {
    const uint8_t val = V[x];
    memory.write(I,     static_cast<uint8_t>(val / 100));
    memory.write(I + 1, static_cast<uint8_t>((val / 10) % 10));
    memory.write(I + 2, static_cast<uint8_t>(val % 10));
}

// ---------------------------------------------------------------------------
//  FX55 - Store V0..VX into memory at I, I+1, ..., I+X
//
//  Using MODERN behavior (I is NOT incremented afterwards). Original
//  CHIP-8 incremented I by X+1. Most modern ROMs expect no increment.
// ---------------------------------------------------------------------------

void CPU::op_Fx55(uint8_t x) {
    for (uint8_t i = 0; i <= x; ++i)
        memory.write(static_cast<uint16_t>(I + i), V[i]);
}

// ---------------------------------------------------------------------------
//  FX65 - Load V0..VX from memory at I, I+1, ..., I+X
// ---------------------------------------------------------------------------

void CPU::op_Fx65(uint8_t x) {
    for (uint8_t i = 0; i <= x; ++i)
        V[i] = memory.read(static_cast<uint16_t>(I + i));
}