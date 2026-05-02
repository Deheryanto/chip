#include <SDL2/SDL.h>
#include <array>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "chip8.h"

const std::array<SDL_Keycode, 16> keymap = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3, // 0,1,2,3
    SDLK_q, SDLK_w, SDLK_e, SDLK_a, // 4,5,6,7
    SDLK_s, SDLK_d, SDLK_z, SDLK_c, // 8,9,A,B
    SDLK_r, SDLK_f, SDLK_v, SDLK_g  // C,D,E,F
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "rom.ch8 \n";
        return 1;
    }

    // SDL initialization
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        64*20, 32*20, SDL_WINDOW_SHOWN);
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, 64, 32);

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    Chip8 chip8;
    if (!chip8.loadROM(argv[1])) {
        std::cerr << "Failed to load ROM\n";
        return 1;
    }

    const int TICKS_PER_FRAME = 10;
    auto lastTime = std::chrono::steady_clock::now();
    bool running  = true;
    SDL_Event event;

    while(running) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count();
        if (elapsed >= 16666) {
            lastTime = now;
            chip8.updateTimers();
        }

        for (int i = 0; i < 10; ++i) {
            chip8.cycle();
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                for (size_t i = 0; i < keymap.size(); ++i) {
                    if (event.key.keysym.sym == keymap[i]) {
                        chip8.keyDown(i);
                        break;
                    }
                }
            } else if (event.type == SDL_KEYUP) {
                for (size_t i = 0; i < keymap.size(); ++i) {
                    if (event.key.keysym.sym == keymap[i]) {
                        chip8.keyUp(i);
                        break;
                    }
                }
            }
        }

        if (chip8.shouldDraw()) {
            uint32_t pixels[64 * 32];
            const auto& fb = chip8.getFramebuffer();
            for (int i = 0; i < 64*32; ++i) {
                pixels[i] = fb[i] ? 0xFFFFFFFF : 0xFF000000;
            }
            SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
            chip8.drawn();
        }

        // TODO : Implement sound
        if (chip8.isSoundBeeping()) {
        }

        int delayTime = 1000 / 60; // 60 FPS
        SDL_Delay(delayTime);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

