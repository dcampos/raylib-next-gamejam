#ifndef GAME_H
#define GAME_H

#include <vector>
#include "raylib.h"

struct Boat {
    int ray;
    float x;
    float y;
    float speed;
};

struct Mine {
    int ray;
    float x;
    float y;
};

struct Fish {
    int ray;
    float x;
    float y;
    float speed;
};

enum GameState {
    STARTING,
    RUNNING,
    PAUSED,
    OVER
};

class Game {
private:
    int screenWidth;
    int screenHeight;

    RenderTexture2D target;
    Texture2D wavesTexture;
    Texture2D waveTexture;
    Texture2D boatTexture;
    Texture2D boatTextureFlipped;
    Texture2D mineTexture;
    Texture2D fishTexture, fishTextureFlipped;
    Sound explosionSound;
    Sound pickupSound;

    std::vector<Mine> mines;
    std::vector<Fish> fish;

    // Input
    bool upPressed;
    bool downPressed;

    float offset;

    Boat boat1, boat2;

    GameState state;
    int score;

    float blinkTimer = 0;

public:
    Game(int screenWidth, int screenHeight);
    ~Game();
    void Reset();
    void IncreaseScore();
    void Finish();
    void SpawnFish();
    void SpawnMine();
    void CheckCollision();
    void UpdateGame(float delta);
    void UpdateDrawFrame();
};

#endif // GAME_H
