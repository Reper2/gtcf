// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif
#include <raylib.h>

#include "headers/alien.h"
#include "headers/backgrounds.h"
#include "headers/colours.h"
#include "headers/penguin.h"
#include "headers/projectile.h"
#include "headers/spaceship.h"

#include "headers/globals.h"

template <typename Entity, typename... Args>
void ExportEntityTexture(
    const char *filename,
    int canvasWidth,
    int canvasHeight,
    float spawnX,
    float spawnY,
    Args &&...args)
{
  RenderTexture2D canvas = LoadRenderTexture(canvasWidth, canvasHeight);

  // The compiler now knows the full layout of Entity!
  Entity entity(spawnX, spawnY, std::forward<Args>(args)...);

  BeginTextureMode(canvas);
  ClearBackground(BLANK);

  entity.display();

  EndTextureMode();

  Image img = LoadImageFromTexture(canvas.texture);
  ImageFlipVertical(&img);

  ExportImage(img, filename);

  UnloadImage(img);
  UnloadRenderTexture(canvas);

  TraceLog(LOG_INFO, "SUCCESS: Exported asset to %s (%dx%d)", filename, canvasWidth, canvasHeight);
}

// Real Memory Allocation
std::string gameState = "START";
unsigned score = 0;
unsigned highscore = 0;
unsigned fleetHealth = 5;

std::vector<Penguin> spacePenguins;
std::vector<Spaceship *> allShips;
std::vector<Spaceship *> deadShips;
std::vector<Alien *> allAliens;
std::vector<Projectile *> projectiles;

std::vector<float> penguinAngles;
std::vector<float> penguinRotSpeeds;

Spaceship *redShip = nullptr;
Spaceship *mainShip = nullptr;
Spaceship *greenShip = nullptr;

Penguin *mainPenguin = nullptr;
Penguin *redPenguin = nullptr;
Penguin *greenPenguin = nullptr;

bool isGameOver = false;
static int lastDamageTime = 0;
static int damageCooldown = 1000;

float defPenguinX;
float defPenguinY;
bool keysHeld[256] = {false};

int lastFireTime = 0;
int fireDelay = 1200;
int lastIceTime = 0;
int iceDelay = 500;

std::vector<Texture2D> loadedBgs;
Texture2D bg;
float bgX;
float bgY;

Font unicodeFont;

const PenguinEye eyeBlueprint = {
    {3.3f, -4.0f, 3.45f},
    {3.5f, -4.0f, 2.07f},
    {3.3f, -4.0f, 2.07f},
    {3.1f, -4.3f, 0.96f}};
auto [ball, iris, pupil, reflect] = eyeBlueprint;

float randomFloat(float min, float max)
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(min, max);
  return dist(gen);
}

float lineX(Vector2 size) noexcept
{
  return virtualWidth / 2.0f - size.x / 2.0f;
};

constexpr unsigned virtualWidth = 1280;
constexpr unsigned virtualHeight = 720;

const float bShipX = virtualWidth / 2.0f;
const std::array<float, 3> shipX = {
    bShipX,
    bShipX - 100,
    bShipX};

const float bShipY = virtualHeight / 2.0f;
const std::array<float, 3> shipY = {
    bShipY / 3.0f,
    bShipY,
    virtualHeight - bShipY / 3.0f};

const std::array<float, 3> shipS = {1.2f, 1.5f, 1.2f};

// Canvas and Render Texture Setup
RenderTexture2D targetCanvas;
const Rectangle sourceRec = {0.0f, 0.0f, (float)virtualWidth, -(float)virtualHeight};
// Initialize destRec dynamically rather than hardcoding 1280x720
Rectangle destRec = {0.0f, 0.0f, (float)virtualWidth, (float)virtualHeight};

#ifdef __EMSCRIPTEN__
EM_BOOL OnBrowserWindowResize(int eventType, const EmscriptenUiEvent *uiEvent, void *userData)
{
  double browserWidth = 0.0, browserHeight = 0.0;
  emscripten_get_element_css_size("#canvas", &browserWidth, &browserHeight);

  if (browserWidth > 0 && browserHeight > 0)
  {
    SetWindowSize((int)browserWidth, (int)browserHeight);
    UpdateViewportScale();
  }
  return EM_TRUE;
}
#endif

void InitViewport()
{
  // 1. Allocate internal high-performance render texture canvas
  targetCanvas = LoadRenderTexture(virtualWidth, virtualHeight);
  SetTextureFilter(targetCanvas.texture, TEXTURE_FILTER_BILINEAR);

#ifdef __EMSCRIPTEN__
  // --- WEB BUILD FIT LOGIC ---
  double browserWidth = 0.0, browserHeight = 0.0;
  // Get window inner dimensions from browser
  emscripten_get_element_css_size("#canvas", &browserWidth, &browserHeight);

  if (browserWidth > 0 && browserHeight > 0)
  {
    SetWindowSize((int)browserWidth, (int)browserHeight);
  }

  // Register dynamic resize listener
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, false, OnBrowserWindowResize);

#else
  // --- NATIVE DESKTOP BUILD FIT LOGIC ---
  int currentMonitor = GetCurrentMonitor();
  int monitorWidth = GetMonitorWidth(currentMonitor);
  int monitorHeight = GetMonitorHeight(currentMonitor);

  // Reserve a 10% safety margin so window chrome/menus/dock fit comfortably
  float scaleFraction = 0.85f;

  // Calculate target height scaled to display
  int winHeight = (int)(monitorHeight * scaleFraction);
  // Maintain 16:9 aspect ratio based on virtualWidth/virtualHeight
  int winWidth = (int)(winHeight * ((float)virtualWidth / (float)virtualHeight));

  // If calculating width exceeds monitor width, clamp to monitor width instead
  if (winWidth > (int)(monitorWidth * scaleFraction))
  {
    winWidth = (int)(monitorWidth * scaleFraction);
    winHeight = (int)(winWidth * ((float)virtualHeight / (float)virtualWidth));
  }

  // Apply new dimensions & center on screen
  SetWindowSize(winWidth, winHeight);
  SetWindowPosition((monitorWidth - winWidth) / 2, (monitorHeight - winHeight) / 2);
#endif

  // 2. Compute letterbox scaling rectangle
  UpdateViewportScale();
}

float viewportZoom = 1.0f;

void UpdateViewportScale() noexcept
{
  const float displayWidth = (float)GetScreenWidth();
  const float displayHeight = (float)GetScreenHeight();

  // Apply the viewportZoom multiplier into the letterbox scale bounds
  const float scale = std::min(displayWidth / (float)virtualWidth, displayHeight / (float)virtualHeight) * viewportZoom;

  destRec.width = (float)virtualWidth * scale;
  destRec.height = (float)virtualHeight * scale;
  destRec.x = (displayWidth - destRec.width) / 2.0f;
  destRec.y = (displayHeight - destRec.height) / 2.0f;
}

void HandleMobilePinchZoom() noexcept
{
  // Check if any pinch gesture is currently active
  int gesture = GetGestureDetected();
  if ((gesture & (GESTURE_PINCH_IN | GESTURE_PINCH_OUT)) != 0)
  {
    // Get the scale vector change (x represents the scale ratio)
    Vector2 pinchVector = GetGesturePinchVector();
    float pinchZoomFactor = pinchVector.x;

    // Raylib returns 0.0f when the pinch is reset or inactive
    if (pinchZoomFactor > 0.0f)
    {
      viewportZoom *= pinchZoomFactor;

      // Clamp the zoom boundaries
      viewportZoom = std::clamp(viewportZoom, 0.4f, 2.5f);

      // Immediately update screen scaling metrics
      UpdateViewportScale();
    }
  }
}

void UnloadViewport() noexcept
{
  UnloadRenderTexture(targetCanvas);
}

void ExportTextures()
{
  // 1. Ensure the 'data/' directory exists on the physical/virtual filesystem
#ifdef __EMSCRIPTEN__
  EM_ASM({
    if (!FS.analyzePath('/data').exists)
    {
      FS.mkdir('/data');
    }
  });
#else
  // Native C++17 directory creation for Windows/macOS/Linux
  std::filesystem::create_directory("data");
#endif

  for (const auto &[name, colour] : colourMap)
  {
    std::string filepath = "data/penguin_" + name + ".png";

    ExportEntityTexture<Penguin>(
        filepath.c_str(),
        512, 512,       // Canvas dimensions
        256.0f, 280.0f, // Spawn position inside canvas
        1.0f,           // Scale factor
        colour          // Corresponding Raylib Color
    );
  }

  for (const auto &[name, colour] : shipColourMap)
  {
    std::string filepath = "data/ship_" + name + ".png";

    ExportEntityTexture<Spaceship>(
        filepath.c_str(),
        1024, 512,
        512.0f, 256.0f,
        2.8f,
        colour,
        nullptr);
  }

  const std::vector<std::string> projectileTypes = {
      "fire",
      "ice"};

  for (const std::string &type : projectileTypes)
  {
    std::string filepath = "data/projectile_" + type + ".png";

    ExportEntityTexture<Projectile>(
        filepath.c_str(),
        64, 64,       // Small canvas size (128x128 fits the ~26px radius projectile easily)
        32.0f, 32.0f, // Center at (64, 64) so it's perfectly framed
        32.0f, 32.0f, // Target X/Y (matches startX/startY so angle is 0)
        type          // Passed directly as the const std::string& type parameter
    );
  }

  ExportEntityTexture<Scout>(
      "data/alien_scout.png",
      128, 128,    // Canvas size
      64.0f, 64.0f // Center position (64, 64)
  );

  ExportEntityTexture<Charger>(
      "data/alien_charger.png",
      128, 128,
      64.0f, 64.0f);

  ExportEntityTexture<Swerver>(
      "data/alien_swerver.png",
      128, 128,
      64.0f, 64.0f);
}

bool ActionPressed(InputAction action)
{
  static const std::unordered_map<InputAction, std::vector<KeyboardKey>> keyMaps = {
      {ACTION_UP, {KEY_W, KEY_UP}},
      {ACTION_DOWN, {KEY_S, KEY_DOWN}},
      {ACTION_LEFT, {KEY_A, KEY_LEFT}},
      {ACTION_RIGHT, {KEY_D, KEY_RIGHT}},
      {ACTION_FIRE, {KEY_Z}},
      {ACTION_ICE, {KEY_X}},
      {ACTION_FLAP_MAIN, {KEY_C}},
      {ACTION_FLAP_RED, {KEY_V}},
      {ACTION_FLAP_GREEN, {KEY_B}}};

  auto it = keyMaps.find(action);
  if (it != keyMaps.end())
  {
    for (KeyboardKey key : it->second)
    {
      if (IsKeyDown(key))
        return true;
    }
  }

  if (action == ACTION_LAUNCH && IsKeyPressed(KEY_R))
    return true;

  // Improved Multi-Touch Gesture / Virtual Pad Scanning
  int touchCount = GetTouchPointCount();
  for (int i = 0; i < touchCount; i++)
  {
    Vector2 physicalTouch = GetTouchPosition(i);

    // Map physical screen touch coordinates directly to virtual resolution space
    float virtX = ((physicalTouch.x - destRec.x) / destRec.width) * virtualWidth;
    float virtY = ((physicalTouch.y - destRec.y) / destRec.height) * virtualHeight;

    // Ignore touches outside the active letterboxed canvas area
    if (virtX < 0 || virtX > virtualWidth || virtY < 0 || virtY > virtualHeight)
      continue;

    // Define broader, friendly touch zones (Left side: Movement control grid, Right side: Fire/Actions)
    if (virtX < (virtualWidth * 0.45f))
    {
      // Left Virtual D-Pad zones
      if (action == ACTION_UP && virtY < (virtualHeight * 0.4f))
        return true;
      if (action == ACTION_DOWN && virtY > (virtualHeight * 0.6f))
        return true;
      if (action == ACTION_LEFT && virtX < (virtualWidth * 0.2f))
        return true;
      if (action == ACTION_RIGHT && virtX >= (virtualWidth * 0.2f) && virtX < (virtualWidth * 0.45f))
        return true;
    }
    else if (virtX > (virtualWidth * 0.55f))
    {
      // Right Action zones (Fire / Ice / Launch)
      if (action == ACTION_FIRE && virtY < (virtualHeight * 0.5f))
        return true;
      if (action == ACTION_ICE && virtY >= (virtualHeight * 0.5f))
        return true;
      if (action == ACTION_LAUNCH)
        return true;
    }
  }
  return false;
}