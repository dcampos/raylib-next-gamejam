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
};

struct Fish {
    int ray;
    float x;
    float speed;
};

class Game {
private:
    int screenWidth;
    int screenHeight;

    RenderTexture2D target;
    Texture2D wavesTexture;
    Texture2D boatTexture;
    Texture2D boatTextureFlipped;
    Texture2D mineTexture;
    Texture2D fishTexture;
    std::vector<Mine> mines;
    std::vector<Fish> fish;

    // Input
    bool upPressed;
    bool downPressed;

    float offset;

    Boat boat1, boat2;

public:
    Game(int screenWidth, int screenHeight);
    ~Game();
    void UpdateDrawFrame();
};

#endif // GAME_H
