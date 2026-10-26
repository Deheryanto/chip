#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "chip8.h"

namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
//  Configuration & Constants
// ---------------------------------------------------------------------------
constexpr int SCREEN_W = 64;
constexpr int SCREEN_H = 32;
constexpr int SCALE    = 20;
constexpr int CPU_HZ   = 700;
constexpr int TIMER_HZ = 60;

constexpr int    AUDIO_SAMPLE_RATE = 44100;
constexpr int    AUDIO_CHANNELS    = 1;
constexpr int    AUDIO_BUFFER_SIZE = 1024;
constexpr double BUZZER_FREQ_HZ    = 440.0;
constexpr double BUZZER_VOLUME     = 0.10;

// Retro Gameboy Theme Colors
constexpr uint32_t COLOR_BG      = 0xFF9BBC0F; // Light Green (Background)
constexpr uint32_t COLOR_ON      = 0xFF0F380F; // Dark Green (Pixels)

// UI Colors (Matching the Gameboy Theme)
constexpr SDL_Color COLOR_TEXT   = {15, 56, 15, 255};    // Dark Green (Unselected text)
constexpr SDL_Color COLOR_SELECT = {0, 0, 0, 255};       // Pitch Black (Selected text)

// ---------------------------------------------------------------------------
//  Keyboard Map
// ---------------------------------------------------------------------------
const std::array<SDL_Keycode, 16> keymap = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3, // 0,1,2,3
    SDLK_q, SDLK_w, SDLK_e, SDLK_a, // 4,5,6,7
    SDLK_s, SDLK_d, SDLK_z, SDLK_c, // 8,9,A,B
    SDLK_r, SDLK_f, SDLK_v, SDLK_g  // C,D,E,F
};

uint8_t sdlKeyToChip8(SDL_Keycode key) {
    for (size_t i = 0; i < keymap.size(); ++i) {
        if (key == keymap[i]) return static_cast<uint8_t>(i);
    }
    return 0xFF;
}

// ---------------------------------------------------------------------------
//  Audio (Square Wave)
// ---------------------------------------------------------------------------
struct AudioState {
    const Chip8* chip8    = nullptr;
    double       phase    = 0.0;
    double       phaseInc = 0.0;
};

void audioCallback(void* userdata, Uint8* stream, int len) {
    auto* state = static_cast<AudioState*>(userdata);
    auto* out   = reinterpret_cast<Sint16*>(stream);
    const int numSamples = len / static_cast<int>(sizeof(Sint16));

    std::memset(stream, 0, static_cast<size_t>(len));

    if (!state || !state->chip8) return;
    if (!state->chip8->isSoundBeeping()) return;

    constexpr Sint16 AMPLITUDE = static_cast<Sint16>(32767 * BUZZER_VOLUME);
    for (int i = 0; i < numSamples; ++i) {
        out[i] = (state->phase < 0.5) ? AMPLITUDE : -AMPLITUDE;
        state->phase += state->phaseInc;
        if (state->phase >= 1.0) state->phase -= 1.0;
    }
}

// ---------------------------------------------------------------------------
//  UI: ROM Selector Menu
// ---------------------------------------------------------------------------
std::vector<std::string> getRomList(const std::string& folder) {
    std::vector<std::string> roms;
    if (!fs::exists(folder)) return roms;

    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            // Accept .ch8 files or files with no extension (classic ROMs)
            if (ext == ".ch8" || ext.empty()) {
                roms.push_back(entry.path().string());
            }
        }
    }
    std::sort(roms.begin(), roms.end());
    return roms;
}

std::string showRomMenu(SDL_Renderer* renderer) {
    std::vector<std::string> roms = getRomList("roms/");
    if (roms.empty()) {
        std::cerr << "Error: 'roms/' folder is empty or not found!\n";
        return "";
    }

    // Load Font (Ensure 'font.ttf' is in the root project folder)
    TTF_Font* font = TTF_OpenFont("font.ttf", 24); 
    if (!font) {
        std::cerr << "\n[ERROR] Failed to load 'font.ttf'. Ensure the file is in the root folder!\n";
        std::cerr << "SDL_ttf Error: " << TTF_GetError() << "\n\n";
        return "";
    }

    int selectedIndex = 0;
    bool inMenu = true;
    std::string chosenRom = "";

    // Ensure logical size is set to the scaled CHIP-8 resolution while in menu
    SDL_RenderSetLogicalSize(renderer, SCREEN_W * SCALE, SCREEN_H * SCALE);

    while (inMenu) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                inMenu = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP && selectedIndex > 0) {
                    selectedIndex--;
                } else if (e.key.keysym.sym == SDLK_DOWN && selectedIndex < static_cast<int>(roms.size()) - 1) {
                    selectedIndex++;
                } else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
                    chosenRom = roms[selectedIndex];
                    inMenu = false;
                } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    inMenu = false;
                }
            }
        }

        // Render Background (Light Green)
        SDL_SetRenderDrawColor(renderer, 155, 188, 15, 255); 
        SDL_RenderClear(renderer);

        // Render Title
        SDL_Surface* titleSurf = TTF_RenderText_Solid(font, "CHIP-8 ROM SELECTOR", COLOR_SELECT);
        if (titleSurf) {
            SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
            SDL_Rect titleRect = {50, 30, titleSurf->w, titleSurf->h};
            SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
            SDL_FreeSurface(titleSurf);
            SDL_DestroyTexture(titleTex);
        }

        // Render List
        int startY = 90;
        int lineHeight = 30;
        int maxVisible = 15;
        
        // Simple scrolling logic
        int offset = 0;
        if (selectedIndex >= maxVisible) {
            offset = selectedIndex - maxVisible + 1;
        }

        for (int i = offset; i < static_cast<int>(roms.size()) && i < offset + maxVisible; ++i) {
            SDL_Color color = (i == selectedIndex) ? COLOR_SELECT : COLOR_TEXT;
            
            std::string filename = fs::path(roms[i]).filename().string();
            if (i == selectedIndex) filename = "> " + filename + " <";

            SDL_Surface* surf = TTF_RenderText_Solid(font, filename.c_str(), color);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect rect = {50, startY + (i - offset) * lineHeight, surf->w, surf->h};
                SDL_RenderCopy(renderer, tex, NULL, &rect);
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);
            }
        }
        
        // Render Instructions
        SDL_Surface* helpSurf = TTF_RenderText_Solid(font, "[UP/DOWN] Navigate  [ENTER] Play  [ESC] Quit", COLOR_TEXT);
        if (helpSurf) {
            SDL_Texture* helpTex = SDL_CreateTextureFromSurface(renderer, helpSurf);
            SDL_Rect helpRect = {50, 550, helpSurf->w, helpSurf->h};
            SDL_RenderCopy(renderer, helpTex, NULL, &helpRect);
            SDL_FreeSurface(helpSurf);
            SDL_DestroyTexture(helpTex);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Restore logical size to original CHIP-8 resolution after exiting menu
    SDL_RenderSetLogicalSize(renderer, SCREEN_W, SCREEN_H);

    TTF_CloseFont(font);
    return chosenRom;
}

// ---------------------------------------------------------------------------
//  Main
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_W * SCALE, SCREEN_H * SCALE,
        SDL_WINDOW_SHOWN);
    if (!window) { 
        std::cerr << "Failed to create window: " << SDL_GetError() << "\n";
        return 1; 
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) { 
        std::cerr << "Failed to create renderer: " << SDL_GetError() << "\n";
        return 1; 
    }
    
    SDL_RenderSetLogicalSize(renderer, SCREEN_W, SCREEN_H);

    SDL_Texture* texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
    if (!texture) { 
        std::cerr << "Failed to create texture: " << SDL_GetError() << "\n";
        return 1; 
    }

    // Determine ROM Path (from argument or UI Menu)
    std::string romPath = "";
    if (argc < 2) {
        // Render a dummy clear screen before showing menu
        SDL_SetRenderDrawColor(renderer, 155, 188, 15, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        romPath = showRomMenu(renderer);
        if (romPath.empty()) {
            // User closed the menu (ESC) or an error occurred
            SDL_DestroyTexture(texture);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
            return 0;
        }
    } else {
        romPath = argv[1];
    }

    // Load Chip8
    Chip8 chip8;
    if (!chip8.loadROM(romPath)) {
        std::cerr << "Failed to load ROM: " << romPath << "\n";
        return 1;
    }

    // Setup Audio
    AudioState audioState;
    audioState.chip8    = &chip8;
    audioState.phaseInc = BUZZER_FREQ_HZ / AUDIO_SAMPLE_RATE;

    SDL_AudioSpec want{}, have{};
    want.freq     = AUDIO_SAMPLE_RATE;
    want.format   = AUDIO_S16SYS;
    want.channels = AUDIO_CHANNELS;
    want.samples  = AUDIO_BUFFER_SIZE;
    want.callback = audioCallback;
    want.userdata = &audioState;

    SDL_AudioDeviceID audioDev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (audioDev != 0) {
        SDL_PauseAudioDevice(audioDev, 0);
    }

    // Game Loop
    using Clock = std::chrono::steady_clock;
    auto last = Clock::now();
    double cpuAcc = 0.0, timerAcc = 0.0;
    const double cpuPeriod   = 1.0 / CPU_HZ;
    const double timerPeriod = 1.0 / TIMER_HZ;

    std::array<uint32_t, SCREEN_W * SCREEN_H> pixels{};
    bool running = true;

    while (running) {
        const auto now = Clock::now();
        const double dt = std::chrono::duration<double>(now - last).count();
        last = now;
        cpuAcc += dt;
        timerAcc += dt;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                uint8_t k = sdlKeyToChip8(event.key.keysym.sym);
                if (k != 0xFF) chip8.keyDown(k);
            } else if (event.type == SDL_KEYUP) {
                uint8_t k = sdlKeyToChip8(event.key.keysym.sym);
                if (k != 0xFF) chip8.keyUp(k);
            }
        }

        while (cpuAcc >= cpuPeriod) {
            chip8.cycle();
            cpuAcc -= cpuPeriod;
        }

        while (timerAcc >= timerPeriod) {
            chip8.updateTimers();
            timerAcc -= timerPeriod;
        }

        if (chip8.shouldDraw()) {
            const auto& fb = chip8.getFramebuffer();
            for (int i = 0; i < SCREEN_W * SCREEN_H; ++i) {
                pixels[i] = fb[i] ? COLOR_ON : COLOR_BG;
            }
            SDL_UpdateTexture(texture, nullptr, pixels.data(), SCREEN_W * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
            chip8.drawn();
        }

        SDL_Delay(1);
    }

    // Cleanup
    if (audioDev != 0) SDL_CloseAudioDevice(audioDev);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}