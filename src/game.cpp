#include <iostream>
#include "game.hpp"
#include "defs.hpp"
#include "util.hpp"

static const Color Blue1 = { 0, 170, 212, 255 };
static const Color Blue2 = { 0, 204, 255, 255 };

Game::Game(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight), upPressed(false),
      downPressed(false), offset(0), target{0} {

    target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    {
        Image image = LoadImage("assets/boat.png");
        boatTexture = LoadTextureFromImage(image);
        ImageFlipHorizontal(&image);
        boatTextureFlipped = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    wavesTexture = LoadTexture("assets/waves.png");
    mineTexture = LoadTexture("assets/mine.png");
    fishTexture = LoadTexture("assets/fish.png");

    // Add mine
    for (int i = 0; i < 1; i++) {
        int x = randomNumber(20, screenWidth - 20);
        int r = randomNumber(1, 6); // Avoid boats for starting mine
        Mine m = { .ray = r, .x = (float)x };
        mines.push_back(m);
    }

    // Add fish
    for (int i = 0; i < 1; i++) {
        int x = randomNumber(20, screenWidth - 20);
        int r = randomNumber(1, 6); // Avoid boats for starting mine
        float speed = r % 2 == 0 ? -WAVE_SPEED : WAVE_SPEED;
        Fish f = { .ray = r, .x = (float)x, .speed = speed };
        fish.push_back(f);
    }

    boat1 = { .ray = 0, .x = 20.0f, .speed = WAVE_SPEED };
    boat2 = { .ray = 7, .x = static_cast<float>(screenWidth - BOAT_WIDTH - 20), .speed = -WAVE_SPEED };
}

// Update and draw frame
void Game::UpdateDrawFrame(void) {

    BeginTextureMode(target);
    ClearBackground(Blue1);

    if (IsKeyPressed(KEY_UP) && boat2.ray > 0) {
        boat1.ray++;
        boat2.ray--;
        boat1.speed *= -1;
        boat2.speed *= -1;
    } else if (IsKeyPressed(KEY_DOWN) && boat1.ray > 0) {
        boat1.ray--;
        boat2.ray++;
        boat1.speed *= -1;
        boat2.speed *= -1;
    }

    // Update boats
    boat1.x += GetFrameTime() * boat1.speed;
    boat2.x += GetFrameTime() * boat2.speed;

    // Update waves
    offset += GetFrameTime() * WAVE_SPEED;
    if (offset > MAX_OFFSET) {
        offset = 0;
    }

    // Draw waves pattern
    for (int i = 0; i < NUM_WAVES; i+=1) {

        int x = i % 2 == 0 ? -offset : offset - MAX_OFFSET;
        Color color = i % 2 == 0 ? Blue2 : Blue1;
        DrawTexture(wavesTexture, x, i * WAVE_HEIGHT + WAVE_OFFSET, color);
        DrawRectangle(0, i * WAVE_HEIGHT + WAVE_HEIGHT + WAVE_OFFSET, screenWidth, screenHeight, color);

        if (i == boat1.ray) {
            auto texture = boat1.speed > 0 ? boatTextureFlipped : boatTexture;
            DrawTexture(texture, boat1.x, i * WAVE_HEIGHT + WAVE_HEIGHT - 5, WHITE);
        }

        if (i == boat2.ray) {
            auto texture = boat2.speed > 0 ? boatTextureFlipped : boatTexture;
            DrawTexture(texture, boat2.x, i * WAVE_HEIGHT + WAVE_HEIGHT - 5, WHITE);
        }

        for (Mine m: mines) {
            if (m.ray == i) {
                DrawTexture(mineTexture, m.x, (m.ray + 1) * WAVE_HEIGHT, WHITE);
            }
        }

        for (Fish f: fish) {
            if (f.ray == i) {
                DrawTexture(fishTexture, f.x, (f.ray + 1) * WAVE_HEIGHT, WHITE);
            }
        }
    }

    EndTextureMode();

    // Render to screen (main framebuffer)
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw render texture to screen, scaled if required
    DrawTexturePro(target.texture,
                   (Rectangle){0, 0, (float)target.texture.width,
                               -(float)target.texture.height},
                   (Rectangle){0, 0, (float)target.texture.width,
                               (float)target.texture.height},
                   (Vector2){0, 0}, 0.0f, WHITE);

    EndDrawing();
}


Game::~Game() {
    UnloadRenderTexture(target);
    UnloadTexture(wavesTexture);
    UnloadTexture(boatTexture);
    UnloadTexture(boatTextureFlipped);
    UnloadTexture(mineTexture);
}
