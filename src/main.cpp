/*******************************************************************************************
*
*   raylib gamejam template
*
*   Template originally created with raylib 4.5-dev, last time updated with raylib 5.0
*
*   Template licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2022-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "defs.hpp"
#include "game.hpp"

// Types and Structures Definition
typedef enum {
    SCREEN_LOGO = 0,
    SCREEN_TITLE,
    SCREEN_GAMEPLAY,
    SCREEN_ENDING
} GameScreen;

// Global Variables Definition
static const int screenWidth = 800;
static const int screenHeight = 520;

static Game* game = nullptr;

// Module Functions Declaration
static void UpdateDrawFrame(void);

// Program main entry point
int main(void) {
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);
#endif

    InitWindow(screenWidth, screenHeight, "Mirror Boats");

    game = new Game(screenWidth, screenHeight);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    delete game;

    CloseWindow();

    return 0;
}

// Update and draw frame
static void UpdateDrawFrame() {
    if (game != nullptr) {
        game->UpdateDrawFrame();
    }
}
