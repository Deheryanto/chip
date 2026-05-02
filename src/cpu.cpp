#include "cpu.h"
#include <random>

CPU::CPU(Memory &mem, Display &display, Keyboard &key, Timer &t)
    : memory(mem), display(display), keyboard(key), timer(t) {
  reset();
}

void CPU::reset() {
  V.fill(0);
  I = 0;
  PC = 0x200;
  stack.fill(0);
  SP = 0;
  memory.loadFont();
}

void CPU::cycle() {
  uint16_t opcode = (memory.read(PC) << 8) | (memory.read(PC + 1));
  PC += 2;
  execute(opcode);
}

void CPU::execute(uint16_t opcode) {
  uint16_t nnn = opcode & 0x0FFF;
  uint16_t n = opcode & 0x000F;
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  uint8_t kk = opcode & 0x00FF;

  switch ((opcode & 0xF000) >> 12) {
  case 0x0:
    if (opcode == 0x00E0)
      clear_screen();
    else if (opcode == 0x00EE)
      op_00EE();
    break;
  case 0x1:
    op_1nnn(nnn);
    break;
  case 0x2:
    op_2nnn(nnn);
    break;
  case 0x3:
    op_3xkk(x, kk);
    break;
  case 0x4:
    op_4xkk(x, kk);
    break;
  case 0x5:
    op_5xy0(x, y);
    break;
  case 0x6:
    op_6xkk(x, kk);
    break;
  case 0x7:
    op_7xkk(x, kk);
    break;
  case 0x8:
    switch (opcode & 0x000F) {
    case 0x0:
      op_8xy0(x, y);
      break;
    case 0x1:
      op_8xy1(x, y);
      break;
    case 0x2:
      op_8xy2(x, y);
      break;
    case 0x3:
      op_8xy3(x, y);
      break;
    case 0x4:
      op_8xy4(x, y);
      break;
    case 0x5:
      op_8xy5(x, y);
      break;
    case 0x6:
      op_8xy6(x, y);
      break;
    case 0x7:
      op_8xy7(x, y);
      break;
    case 0xE:
      op_8xyE(x, y);
      break;
    }
    break;
  case 0x9:
    op_9xy0(x, y);
    break;
  case 0xA:
    op_Annn(nnn);
    break;
  case 0xB:
    op_Bnnn(nnn);
    break;
  case 0xC:
    op_Cxkk(x, kk);
    break;
  case 0xD:
    op_Dxyn(x, y, n);
    break;
  case 0xE:
    if ((opcode & 0x00FF) == 0x009E)
      op_Ex9E(x);
    else if ((opcode & 0x00FF) == 0x00A1)
      op_ExA1(x);
    break;
  case 0xF:
    switch (opcode & 0x00FF) {
    case 0x0007:
      op_Fx07(x);
      break;
    case 0x000A:
      op_Fx0A(x);
      break;
    case 0x0015:
      op_Fx15(x);
      break;
    case 0x0018:
      op_Fx18(x);
      break;
    case 0x001E:
      op_Fx1E(x);
      break;
    case 0x0029:
      op_Fx29(x);
      break;
    case 0x0033:
      op_Fx33(x);
      break;
    case 0x0055:
      op_Fx55(x);
      break;
    case 0x0065:
      op_Fx65(x);
      break;
    }
    break;
  default:
    break;
  }
}

void CPU::clear_screen() {
  display.clear();
  display.requestDraw();
}

void CPU::op_00EE() {
  if (SP > 0) {
    SP--;
    PC = stack[SP];
  }
}

void CPU::op_1nnn(uint16_t nnn) { PC = nnn; }

void CPU::op_2nnn(uint16_t nnn) {
  if (SP < 16) {
    stack[SP] = PC;
    SP++;
    PC = nnn;
  }
}

void CPU::op_3xkk(uint8_t x, uint8_t kk) {
  if (V[x] == kk) {
    PC += 2;
  }
}

void CPU::op_4xkk(uint8_t x, uint8_t kk) {
  if (V[x] != kk)
    PC += 2;
}

void CPU::op_5xy0(uint8_t x, uint8_t y) {
  if (V[x] == V[y])
    PC += 2;
}

void CPU::op_6xkk(uint8_t x, uint8_t kk) { V[x] = kk; }

void CPU::op_7xkk(uint8_t x, uint8_t kk) { V[x] += kk; }

void CPU::op_8xy0(uint8_t x, uint8_t y) { V[x] = V[y]; }

void CPU::op_8xy1(uint8_t x, uint8_t y) { V[x] |= V[y]; }

void CPU::op_8xy2(uint8_t x, uint8_t y) { V[x] &= V[y]; }

void CPU::op_8xy3(uint8_t x, uint8_t y) { V[x] ^= V[y]; }

void CPU::op_8xy4(uint8_t x, uint8_t y) {
  uint16_t sum = V[x] + V[y];
  V[0xF] = (sum > 0xFF) ? 1 : 0;
  V[x] = sum & 0xFF;
}

void CPU::op_8xy5(uint8_t x, uint8_t y) {
  V[0xF] = (V[x] >= V[y]) ? 1 : 0;
  V[x] -= V[y];
}

void CPU::op_8xy6(uint8_t x, uint8_t y) {
  V[0xF] = V[x] & 0x1;
  V[x] >>= 1;
}

void CPU::op_8xy7(uint8_t x, uint8_t y) {
  V[0xF] = (V[y] >= V[x]) ? 1 : 0;
  V[x] = V[y] - V[x];
}

void CPU::op_8xyE(uint8_t x, uint8_t y) {
  V[0xF] = (V[x] & 0x80) >> 7;
  V[x] <<= 1;
}

void CPU::op_9xy0(uint8_t x, uint8_t y) {
  if (V[x] != V[y])
    PC += 2;
}

void CPU::op_Annn(uint16_t nnn) { I = nnn; }

void CPU::op_Bnnn(uint16_t nnn) { PC = nnn + V[0x0]; }

void CPU::op_Cxkk(uint8_t x, uint8_t kk) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<uint8_t> dist(0, 255);
  V[x] = dist(gen) & kk;
}

void CPU::op_Dxyn(uint8_t x, uint8_t y, uint8_t n) {
  uint8_t x0 = V[x] % Display::WIDTH;
  uint8_t y0 = V[y] % Display::HEIGHT;
  V[0xF] = 0;

  for (uint8_t row = 0; row < n; ++row) {
    if (y0 + row >= Display::HEIGHT)
      break;

    uint8_t sprite_byte = memory.read(I + row);

    for (uint8_t col = 0; col < 8; ++col) {
      if (x0 + col >= Display::WIDTH)
        break;

      bool sprite_pixel = (sprite_byte & (0x80 >> col)) != 0;
      if (sprite_pixel) {
        bool current_pixel = display.getPixel(x0 + col, y0 + row);
        if (current_pixel) {
          V[0xF] = 1; // Collision
        }

        display.setPixel(x0 + col, y0 + row, current_pixel ^ sprite_pixel);
      }
    }
  }
  display.requestDraw();
}

void CPU::op_Ex9E(uint8_t x) {
  if (keyboard.isKeyPressed(V[x])) {
    PC += 2;
  }
}

void CPU::op_ExA1(uint8_t x) {
  if (!keyboard.isKeyPressed(V[x]))
    PC += 2;
}

void CPU::op_Fx07(uint8_t x) { V[x] = timer.getDelay(); }

void CPU::op_Fx0A(uint8_t x) {
  uint8_t key = keyboard.waitForKey();

  if (key != 0xFF) {
    V[x] = key;
  } else {
    PC -= 2;
  }
}

void CPU::op_Fx15(uint8_t x) { timer.setDelay(V[x]); }

void CPU::op_Fx18(uint8_t x) { timer.setSound(V[x]); }

void CPU::op_Fx1E(uint8_t x) { I += V[x]; }

void CPU::op_Fx29(uint8_t x) {
  I = V[x] * 5; // each font is 5 bytes
}

void CPU::op_Fx33(uint8_t x) {
  uint8_t val = V[x];
  memory.write(I, val / 100);
  memory.write(I + 1, (val / 10) % 10);
  memory.write(I + 2, val % 10);
}

// Store registers V0..Vx into memory starting at I
void CPU::op_Fx55(uint8_t x) {
  for (uint8_t i = 0; i <= x; ++i) {
    memory.write(I + i, V[i]);
  }
}

// Load registers V0..Vx from memory starting at I
void CPU::op_Fx65(uint8_t x) {
  for (uint8_t i = 0; i <= x; ++i) {
    V[i] = memory.read(I + i);
  }
}
