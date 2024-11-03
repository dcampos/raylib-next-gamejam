#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include "game.hpp"
#include "defs.hpp"
#include "raylib.h"
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
    waveTexture = LoadTexture("assets/wave.png");
    mineTexture = LoadTexture("assets/mine.png");

    {
        Image image = LoadImage("assets/fish.png");
        fishTexture = LoadTextureFromImage(image);
        ImageFlipHorizontal(&image);
        fishTextureFlipped = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    pickupSound = LoadSound("assets/pickup.wav");
    explosionSound = LoadSound("assets/explosion.wav");

    LoadScore();
    std::cout << "best score: " << bestScore << std::endl;

    state = STARTING;
}

void Game::Reset() {
    bestScore = score > bestScore ? score : bestScore;

    state = RUNNING;
    score = 0;

    mines.clear();
    fish.clear();

    // Add mine
    for (int i = 0; i < 1; i++) {
        SpawnMine();
    }

    // Add fish
    for (int i = 0; i < 1; i++) {
        SpawnFish();
    }

    boat1 = { .ray = 0, .x = 20.0f, .speed = WAVE_SPEED };
    boat2 = { .ray = 7, .x = static_cast<float>(screenWidth - BOAT_WIDTH - 20), .speed = -WAVE_SPEED };

    blinkTimer = 0;
}

void Game::IncreaseScore() {
    score += 10;
    std::cout << "Score is now: " << score << std::endl;
}

void Game::Finish() {
    state = OVER;
    SaveScore();
}

void Game::SpawnFish() {
    // int x = randomNumber(20, screenWidth - 20);
    int r = randomNumber(1, 6); // Avoid boats for starting
    float speed = r % 2 == 0 ? WAVE_SPEED : -WAVE_SPEED;
    int x = r % 2 == 0 ? -20 : screenWidth + 20;
    Fish f = { .ray = r, .x = (float)x, .speed = speed };
    fish.push_back(f);
}

void Game::SpawnMine() {
    int x = randomNumber(20, screenWidth - 20);
    int r;
    do {
        r = randomNumber(0, 7);
        // Avoid starting on the same ray as a boat
    } while (boat1.ray == r || boat2.ray == r);
    Mine m = { .ray = r, .x = (float)x };
    mines.push_back(m);
}

void Game::CheckCollision() {
    // Check for boat->fish collision
    for (int i = fish.size() - 1; i >= 0; i--) {
        Fish& f = fish[i];
        bool collided = false;
        if (f.ray == boat1.ray) {
            if (inRange(f.x, boat1.x, BOAT_WIDTH) || inRange(boat1.x, f.x, FISH_WIDTH)) {
                collided = true;
            }
        } else if (f.ray == boat2.ray) {
            if (inRange(f.x, boat2.x, BOAT_WIDTH) || inRange(boat2.x, f.x, FISH_WIDTH)) {
                collided = true;
            }
        }

        if (collided) {
            fish.erase(fish.begin() + i);
            IncreaseScore();
            SpawnFish();
            SpawnMine();
            PlaySound(pickupSound);
        }
    }

    for (Mine& m: mines) {
        bool collided = false;
        if (m.ray == boat1.ray) {
            if (inRange(m.x, boat1.x, BOAT_WIDTH) || inRange(boat1.x, m.x, MINE_WIDTH / 2.f)) {
                collided = true;
            }
        } else if (m.ray == boat2.ray) {
            if (inRange(m.x, boat2.x, BOAT_WIDTH) || inRange(boat2.x, m.x, MINE_WIDTH / 2.f)) {
                collided = true;
            }
        }
        if (collided) {
            PlaySound(explosionSound);
            Finish();
        }
    }
}

void Game::UpdateGame(float delta) {
    CheckCollision();

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

    for (int i = fish.size() - 1; i >= 0; i--) {
        Fish& f = fish[i];
        f.x += delta * f.speed;
        if (f.speed > 0 && f.x > screenWidth || (f.speed < 0 && f.x - FISH_WIDTH < 0)) {
            fish.erase(fish.begin() + i);
            SpawnFish();
        }
    }

    // Update boats
    boat1.x += delta * boat1.speed;
    boat2.x += delta * boat2.speed;

    if (boat1.x + BOAT_WIDTH < 0 || boat1.x > screenWidth) {
        // Checking only one boat is enough
        Finish();
    }
}

// Update and draw frame
void Game::UpdateDrawFrame(void) {
    BeginTextureMode(target);
    ClearBackground(Blue1);

    if (IsKeyPressed(KEY_SPACE)) {
        Reset();
    }

    Color tint;
    if (state == RUNNING) {
        UpdateGame(GetFrameTime());
        tint = WHITE;
    } else {
        tint = BLANK;
    }

    // Always update waves
    offset += GetFrameTime() * WAVE_SPEED;
    if (offset > MAX_OFFSET) {
        offset = 0;
    }

    // Draw waves pattern
    for (int i = 0; i < NUM_WAVES; i+=1) {

        int x = i % 2 == 0 ? -offset : offset - MAX_OFFSET;
        Color color = i % 2 == 0 ? Blue2 : Blue1;

        int limit = screenWidth / WAVE_WIDTH + MAX_OFFSET;
        for (int j = 0; j < limit; j++) {
            DrawTexture(waveTexture, x + (j * WAVE_WIDTH), i * WAVE_HEIGHT + WAVE_OFFSET, color);
        }
        DrawRectangle(0, i * WAVE_HEIGHT + WAVE_HEIGHT + WAVE_OFFSET, screenWidth, screenHeight, color);

        if (i == boat1.ray) {
            auto texture = boat1.speed > 0 ? boatTextureFlipped : boatTexture;
            DrawTexture(texture, boat1.x, i * WAVE_HEIGHT + WAVE_HEIGHT - 5, tint);
        }

        if (i == boat2.ray) {
            auto texture = boat2.speed > 0 ? boatTextureFlipped : boatTexture;
            DrawTexture(texture, boat2.x, i * WAVE_HEIGHT + WAVE_HEIGHT - 5, tint);
        }

        for (Mine m: mines) {
            if (m.ray == i) {
                DrawTexture(mineTexture, m.x, (m.ray + 1) * WAVE_HEIGHT, tint);
            }
        }

        for (Fish f: fish) {
            auto texture = f.speed > 0 ? fishTextureFlipped : fishTexture;
            if (f.ray == i) {
                DrawTexture(texture, f.x, (f.ray + 1) * WAVE_HEIGHT + 5, tint);
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

    if (state == STARTING) {
        float ry = screenHeight / 4.f;
        DrawRectangle(0, ry, screenWidth, screenHeight / 2, BLACK);

        blinkTimer += GetFrameTime();

        if (blinkTimer < 0.5f) {
            std::string playAgainText = "Press SPACE to start";
            float textWidth = MeasureText(playAgainText.c_str(), 28);
            DrawText(playAgainText.c_str(), screenWidth / 2.f - textWidth / 2, screenHeight / 2.f - 14, 28, WHITE);
        }

        if (blinkTimer > 1.0f) {
            blinkTimer = 0;
        }
    } else if (state == OVER) {
        float ry = screenHeight / 4.f;
        DrawRectangle(0, ry, screenWidth, screenHeight / 2, BLACK);

        std::string gameOverText = "GAME OVER";
        float textWidth = MeasureText(gameOverText.c_str(), 38);
        DrawText(gameOverText.c_str(), screenWidth / 2.f - textWidth / 2, ry + 10, 38, WHITE);

        std::string scoreText = "Score: " + std::to_string(score);
#if not defined(PLATFORM_WEB)
        if (score > bestScore) {
            scoreText += " *NEW BEST*";
        } else {
            scoreText += " | Best: " + std::to_string(bestScore);
        }
#endif
        textWidth = MeasureText(scoreText.c_str(), 28);
        DrawText(scoreText.c_str(), screenWidth / 2.f - textWidth / 2, ry + screenHeight / 2.f - 35, 28, WHITE);

        blinkTimer += GetFrameTime();

        if (blinkTimer < 0.5f) {
            std::string playAgainText = "Press SPACE to play again";
            textWidth = MeasureText(playAgainText.c_str(), 28);
            DrawText(playAgainText.c_str(), screenWidth / 2.f - textWidth / 2, screenHeight / 2.f - 14, 28, WHITE);
        }

        if (blinkTimer > 1.0f) {
            blinkTimer = 0;
        }
    } else {
        DrawText(TextFormat("Score: %i", score), 20, 10, 28, WHITE);
    }

    EndDrawing();
}

void Game::LoadScore() {
    int bytesRead;
    unsigned char* data = LoadFileData("save.dat", &bytesRead);
    if (bytesRead == sizeof(int)) {
        bestScore = *(int*)data;
    }
    UnloadFileData(data);
}

void Game::SaveScore() {
    int saveScore = std::max(bestScore, score);
    SaveFileData("save.dat", &bestScore, sizeof(int));
}

Game::~Game() {
    UnloadRenderTexture(target);
    UnloadTexture(wavesTexture);
    UnloadTexture(boatTexture);
    UnloadTexture(boatTextureFlipped);
    UnloadTexture(mineTexture);
    UnloadTexture(fishTexture);
    UnloadTexture(fishTextureFlipped);
    UnloadSound(pickupSound);
    UnloadSound(explosionSound);
}
