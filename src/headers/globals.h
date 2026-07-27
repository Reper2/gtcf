// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1


#pragma once
#include <array>
#include <string>
#include <type_traits>
#include <vector>
#include <raylib.h>

// Forward declarations so the header recognises the types before they fully parse
class Penguin;
class Spaceship;
class Projectile;
class Alien;
class Scout;
class Charger;
class Swerver;

/**
 * Generic Off-Screen Texture Exporter
 * Works for Penguin, Spaceship, Alien, or any object implementing .display()
 */
template <typename Entity, typename... Args>
void ExportEntityTexture(
    const char* filename,
    int canvasWidth,
    int canvasHeight,
    float spawnX,
    float spawnY,
    Args&&... args);

// --- SHARED ENVIRONMENT STATES ---
extern std::string gameState;
extern unsigned score;
extern unsigned highscore;
extern unsigned fleetHealth;
extern bool isGameOver;

// --- SCREEN CONSTRAINTS ---
extern const unsigned virtualWidth;
extern const unsigned virtualHeight;
extern float viewportZoom;

// --- DYNAMIC GAME OBJECT LISTS ---
extern std::vector<Penguin> spacePenguins;
extern std::vector<Spaceship *> allShips;
extern std::vector<Spaceship *> deadShips;

extern Spaceship *redShip;
extern Spaceship *mainShip;
extern Spaceship *greenShip;

// Shared pointers to the explicit main pilot allocations
extern Penguin *mainPenguin;
extern Penguin *redPenguin;
extern Penguin *greenPenguin;

// Background penguin physics tracking vectors
extern std::vector<float> penguinAngles;
extern std::vector<float> penguinRotSpeeds;

extern std::vector<Alien *> allAliens;

// --- TIMERS & INPUT MECHANICS ---
extern float defPenguinX;
extern float defPenguinY;
extern bool keysHeld[256];
extern std::vector<Projectile *> projectiles;

extern int lastFireTime;
extern int fireDelay;
extern int lastIceTime;
extern int iceDelay;

// --- GRAPHICAL ASSETS ---
extern std::vector<Texture2D> loadedBgs;
extern Texture2D bg;
extern float bgX;
extern float bgY;
extern Font unicodeFont;

// The core components of a single eye feature
struct EyeComponent
{
    float x;
    float y;
    float size;
};

// The complete set of layers that form a penguin eye cluster
struct PenguinEye
{
    EyeComponent ball;
    EyeComponent iris;
    EyeComponent pupil;
    EyeComponent reflect;
};

extern const PenguinEye eyeBlueprint;

extern float randomFloat(float min, float max);
extern float lineX(Vector2 size) noexcept;

extern const float bShipX;
extern const std::array<float, 3> shipX;

extern const float bShipY;
extern const std::array<float, 3> shipY;

extern const std::array<float, 3> shipS;

extern RenderTexture2D targetCanvas;
extern const Rectangle sourceRec;
extern Rectangle destRec;

void InitViewport();
void UpdateViewportScale() noexcept;
void HandleMobilePinchZoom() noexcept;
void UnloadViewport() noexcept;

void ExportTextures();

enum InputAction
{
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_FIRE,
    ACTION_ICE,
    ACTION_FLAP_MAIN,
    ACTION_FLAP_RED,
    ACTION_FLAP_GREEN,
    ACTION_LAUNCH
};

bool ActionPressed(InputAction action);