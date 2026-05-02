# 🕹️ Chip-8 Emulator (C++17 & SDL2)

![C++](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)
![SDL2](https://img.shields.io/badge/Library-SDL2-green.svg)
![Build](https://img.shields.io/badge/Build-CMake-orange.svg)

Implementasi emulator sistem interpretatif **Chip-8** yang dibangun dari nol menggunakan C++ modern. Proyek ini mensimulasikan arsitektur virtual machine dari tahun 1970-an yang berjalan pada sistem seperti COSMAC VIP dan Telmac 1800.

---

## 🚀 Fitur Utama
- **Core CPU Akurat:** Implementasi lengkap 35 opcodes Chip-8.
- **Sistem Memori:** Simulasi RAM 4KB dengan pemetaan alamat yang tepat.
- **Display Modern:** Rendering grafis 64x32 menggunakan **SDL2** dengan fitur penskalaan jendela.
- **Audio & Timer:** Implementasi sistem *Delay Timer* dan *Sound Timer* (60Hz).
- **Input Keypad:** Pemetaan 16-key hexadecimal asli ke keyboard PC modern.
- **Cross-Platform:** Kompatibel dengan Linux, Windows, dan macOS melalui CMake.

---

## 🛠️ Arsitektur Teknis
Proyek ini diorganisir secara modular untuk mencerminkan komponen perangkat keras asli:
- **`CPU`**: Menangani siklus *fetch-decode-execute*.
- **`Memory`**: Mengelola RAM, fontset, dan pemuatan ROM.
- **`Display`**: Mengelola framebuffer dan logika pixel XOR.
- **`Keyboard`**: Menangani status input dari user.
- **`Timer`**: Mengelola sinkronisasi waktu untuk logika game dan suara.

---

## 📋 Prasyarat
Sebelum melakukan kompilasi, pastikan sistem Anda memiliki:
- **CMake** (v3.10 atau lebih tinggi)
- **C++ Compiler** (mendukung standar C++17)
- **SDL2 Library**
  - Ubuntu/Debian: `sudo apt install libsdl2-dev`
  - macOS: `brew install sdl2`

---

## ⚙️ Kompilasi dan Instalasi
Gunakan perintah berikut di terminal Anda:

```bash
# 1. Kloning repositori
git clone [https://github.com/Deheryanto/chip8.git](https://github.com/Deheryanto/chip8.git)
cd chip8

# 2. Buat direktori build
mkdir build && cd build

# 3. Generate Makefile dan Build
cmake ..
make
```

Setelah selesai, executable bernama chip8 akan muncul di folder build/.

---

## 🎮 Cara Menjalankan Game

Beberapa Rom sudah tersedia pada folder roms
```bash
./chip8 ../roms/nama rom


