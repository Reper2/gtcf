// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1

#include <raylib.h>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <fstream>
#include <filesystem>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "headers/globals.h"
#include "headers/key_input.h"
#include "headers/penguin.h"
#include "headers/spaceship.h"
#include "headers/alien.h"
#include "headers/projectile.h"
#include "headers/colours.h"
#include "headers/backgrounds.h"

void LoadHighScore()
{
#ifdef __EMSCRIPTEN__
  static bool filesystemMounted = false;
  if (!filesystemMounted)
  {
    EM_ASM({
      var idbfs = (typeof IDBFS !== 'undefined') ? IDBFS : (FS.filesystems ? FS.filesystems.IDBFS : null);
      if (idbfs)
      {
        try
        {
          FS.mkdir('/savestate');
        }
        catch (e)
        {
        }
        FS.mount(idbfs, {}, '/savestate');
      }
      else
      {
        console.error("IDBFS library not available.");
      }
    });
    filesystemMounted = true;
  }

  EM_ASM({
    FS.syncfs(true, function(err) {
      if (err) console.error("Error loading save from IndexedDB:", err); });
  });
#endif

  std::string savePath = "data/highscore.dat";
#ifdef __EMSCRIPTEN__
  savePath = "/savestate/highscore.dat";
#endif

  std::ifstream scoreFile(savePath, std::ios::binary);
  if (scoreFile.is_open())
  {
    int loadedScore = 0;
    scoreFile.read(reinterpret_cast<char *>(&loadedScore), sizeof(loadedScore));
    scoreFile.close();

    // Sanity check: If loaded score is negative or unreasonably huge garbage data, reset to 0
    if (loadedScore < 0 || loadedScore > 1000000)
    {
      highscore = 0;
    }
    else
    {
      highscore = static_cast<unsigned>(loadedScore);
    }
  }
  else
  {
    highscore = 0;
  }
}

void SaveHighScore()
{
  std::string savePath = "data/highscore.dat";
#ifdef __EMSCRIPTEN__
  savePath = "/savestate/highscore.dat";
#endif

  std::ofstream scoreFile(savePath, std::ios::binary | std::ios::trunc);
  if (scoreFile.is_open())
  {
    scoreFile.write(reinterpret_cast<const char *>(&highscore), sizeof(highscore));
    scoreFile.close();

#ifdef __EMSCRIPTEN__
    // Sync memory state back down to IndexedDB (false = virtual MEMFS -> IndexedDB)
    EM_ASM({
      FS.syncfs(false, function(err) {
        if (err) console.error("Error saving to IndexedDB:", err); });
    });
#endif
  }
}

void ChangeGameState(std::string newState)
{
  gameState = newState;

  if (gameState == "START")
  {
    bg = loadedBgs[0];
  }
  else if (gameState == "PLAY")
  {
    bg = loadedBgs[GetRandomBackgroundIndex()];
  }
  else if (gameState == "GAMEOVER")
  {
    bg = loadedBgs[GetRandomBackgroundIndex()];
  }

  bgX = ((float)GetScreenWidth() - bg.width) / 2.0f;
  bgY = ((float)GetScreenHeight() - bg.height) / 2.0f;
}

void spawnSpacePenguins()
{
  // Scatter fresh penguins across the actual allocated background map dimensions
  constexpr int bgPenguins = 30;
  for (int i = 0; i < bgPenguins; i++)
  {
    const float randX = randomFloat(0, bg.width);
    const float randY = randomFloat(0, bg.height);

    spacePenguins.push_back(Penguin(randX, randY, randomFloat(12, 25) / 100.0f, blueRGBA, 60));
    penguinAngles.push_back(randomFloat(0, 360) * DEG2RAD);
    penguinRotSpeeds.push_back((randomFloat(-30, 30)) / 10000.0f);
  }
}

void setup()
{
  // Clear out ambient world-space penguins entirely so they don't stack up
  spacePenguins.clear();
  penguinAngles.clear();
  penguinRotSpeeds.clear();

  // Select the mission background slice and populate the drifting space penguins
  bg = loadedBgs[randomFloat(0, loadedBgs.size() - 1)];
  spawnSpacePenguins();

  // Safely purge existing global pilot memory allocations to prevent leaks
  delete mainPenguin;
  delete redPenguin;
  delete greenPenguin;

  // Instantiate FRESH new Pilot Entities
  mainPenguin = new Penguin(defPenguinX, defPenguinY, 0.75f, blueRGBA);
  redPenguin = new Penguin(defPenguinX + 150.0f, defPenguinY, 0.5f, redRGBA);
  greenPenguin = new Penguin(defPenguinX - 150.0f, defPenguinY, 0.5f, greenRGBA);

  // Clear out previous spaceship tracker array layers
  for (Spaceship *ship : allShips)
  {
    delete ship;
  }
  allShips.clear();

  for (Spaceship *ds : deadShips)
  {
    delete ds;
  }
  deadShips.clear();

  // Reset pointers to clean safety states
  redShip = nullptr;
  mainShip = nullptr;
  greenShip = nullptr;

  // Rebuild the Squadron Layout Vector mapping fresh configurations
  allShips.push_back(redShip = new Spaceship(shipX[0], shipY[0], shipS[0], marsPioneerStyle, redPenguin));
  allShips.push_back(mainShip = new Spaceship(shipX[1], shipY[1], shipS[1], grandTourStyle, mainPenguin));
  allShips.push_back(greenShip = new Spaceship(shipX[2], shipY[2], shipS[2], enceledusStyle, greenPenguin));
}

void resetGame()
{
  score = 0;
  fleetHealth = 5;

  // Clear any weapons projectiles floating in space from the last round
  for (Projectile *proj : projectiles)
  {
    delete proj;
  }
  projectiles.clear();

  // Purge remaining active alien arrays
  for (Alien *alien : allAliens)
  {
    delete alien;
  }
  allAliens.clear();

  // Execute unified pipeline to spawn penguins, pilots, and ships cleanly!
  setup();

  ChangeGameState("PLAY");
}

int main()
{
  SetRandomSeed(time(NULL));
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
  InitWindow(1280, 720, "Grand Tour: Cold Front");
  InitViewport();
  SetTargetFPS(60);

  LoadHighScore();

#ifndef __EMSCRIPTEN__
  // Only generate & write PNGs to disk when running desktop native builds!
  ExportTextures();
#endif

  std::vector<int> codepoints;
  for (unsigned i = 32; i < 127; i++)
  {
    codepoints.push_back(i);
  }
  codepoints.push_back(0x2665); // Heart
  codepoints.push_back(0x2022); // Bullet Dot
  codepoints.push_back(0x00A9); // (c)

  unicodeFont = LoadFontEx("resources/fonts/arial.ttf", 32, codepoints.data(), codepoints.size());
  SetTextureFilter(unicodeFont.texture, TEXTURE_FILTER_BILINEAR);

  const Font titleFont = LoadFontEx("resources/fonts/HyliaSerifBeta-Regular.otf", 40, codepoints.data(), 95);
  SetTextureFilter(titleFont.texture, TEXTURE_FILTER_BILINEAR);

  for (size_t i = 0; i < BACKGROUND_IMAGES.size(); i++)
  {
    const std::string path = GetBackgroundFilePath(i);
    const Texture2D tex = LoadTexture(path.c_str());
    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
    loadedBgs.push_back(tex);
  }

  setup();
  unsigned frameCount = 0;

  while (!WindowShouldClose())
  {
    frameCount++;

    if (IsWindowResized())
    {
      bgX = ((float)GetScreenWidth() - bg.width) / 2.0f;
      bgY = ((float)GetScreenHeight() - bg.height) / 2.0f;
    }

    // 1. Flush and capture input buffer
    for (unsigned i = 0; i < 256; i++)
    {
      keysHeld[i] = false;
    }
    for (unsigned key = 32; key < 126; key++)
    {
      if (IsKeyDown(key))
      {
        char lowerKey = std::tolower(key);
        keysHeld[static_cast<unsigned char>(lowerKey)] = true;
      }
    }

    // Keep them moving lazily on the title menu screen
    for (size_t i = 0; i < spacePenguins.size(); i++)
    {
      Penguin &p = spacePenguins[i];

      p.x += 0.08f;
      p.y += std::sin(frameCount * 0.01f + (i * 10.0f)) * 0.05f;

      penguinAngles[i] += penguinRotSpeeds[i];
      p.slideAngle = penguinAngles[i];

      const float rightBoundary = (float)bg.width + 60.0f;
      if (p.x > rightBoundary)
      {
        p.x = -60.0f;
        p.y = randomFloat(0, bg.height);
      }
    }

    if (gameState == "PLAY")
    {
      // Only pass the flagship input if it hasn't been completely vaporized!
      if (mainShip != nullptr)
      {
        handleMainInp(mainShip, "wsadzx");
      }

      // Safeguard all individual pilot inputs
      if (mainPenguin != nullptr)
      {
        handlePilotInp(mainPenguin, "czx");
      }
      if (redPenguin != nullptr)
      {
        handlePilotInp(redPenguin, "vzx");
      }
      if (greenPenguin != nullptr)
      {
        handlePilotInp(greenPenguin, "bzx");
      }

      for (Spaceship *ship : allShips)
      {
        ship->update();
      }

      // Draw the drifting dead ship remains behind or ahead of active ships
      for (Spaceship *ds : deadShips)
      {
        ds->display();
      }

      Projectile::processGlobalCollisions();

      // Update dead drifting ships backward
      for (int i = deadShips.size() - 1; i >= 0; i--)
      {
        Spaceship *ds = deadShips[i];

        // Drift the metal ship backward quickly
        ds->x -= 3.5f;

        // Make its pilot penguin break loose and drift out of the cockpit frame slowly
        if (ds->pilot != nullptr)
        {
          ds->pilot->x -= 1.2f;           // Slower backward drift than the ship
          ds->pilot->y -= 0.3f;           // Slight float upwards into space
          ds->pilot->slideAngle += 0.04f; // Tumbling spin out animation
        }

        ds->update(); // Keeps its particle engine trails animating while it drifts away!

        // Screen-culling check: If BOTH the ship and the pilot have cleared the left boundary, clean memory!
        bool shipOffScreen = (ds->x < -200.0f);
        bool pilotOffScreen = (ds->pilot == nullptr || ds->pilot->x < -100.0f);

        if (shipOffScreen && pilotOffScreen)
        {
          // Clean up the dynamically allocated pilot memory first
          if (ds->pilot != nullptr)
          {
            delete ds->pilot;
            ds->pilot = nullptr;
          }

          delete ds;
          deadShips.erase(deadShips.begin() + i);
        }
      }

      for (size_t i = 0; i < spacePenguins.size(); i++)
      {
        Penguin &p = spacePenguins[i];
        // Slow ambient space drift (relative to the background canvas size!)
        p.x += 0.08f;
        p.y += std::sin(frameCount * 0.01f + (i * 10.0f)) * 0.05f;

        // Simple tumbling rotation
        penguinAngles[i] += penguinRotSpeeds[i];
        p.slideAngle = penguinAngles[i];

        float rightBoundary = (float)bg.width + 60.0f;
        float leftReset = -60.0f;
        if (p.x > rightBoundary)
        {
          p.x = leftReset;
          p.y = randomFloat(0, bg.height);
        }
      }

      if (allAliens.size() == 0)
      {
        const int randNum = static_cast<int>(randomFloat(0, loadedBgs.size()));
        bg = loadedBgs[randNum];
        // Caps the maximum spawn at 24 aliens, and rounds to a whole number
        const unsigned waveSize = std::min<unsigned>(24, 2 * (1 + score / 1000));
        spawnAlienWave(waveSize, allShips);
      }

      for (Alien *alien : allAliens)
      {
        alien->update(allShips);
        alien->display();
      }

      if (fleetHealth <= 0 && gameState == "PLAY")
      {
        // New record verification
        if (score > highscore)
        {
          highscore = score;
          SaveHighScore(); // Write the new record to your binary save file!
        }
        ChangeGameState("GAMEOVER");
      }
    }
    else if (gameState == "START")
    {
      if (keysHeld['r'] || IsKeyPressed(KEY_R) || GetTouchPointCount() > 0)
      {
        ChangeGameState("PLAY");
      }
    }
    else if (gameState == "GAMEOVER")
    {
      if (keysHeld['r'] || IsKeyPressed(KEY_R) || GetTouchPointCount() > 0)
      {
        resetGame();
        ChangeGameState("PLAY");
      }
    }

    // --- RENDER STEP ---
    BeginTextureMode(targetCanvas);
    ClearBackground(BLACK);

    // Draw deep space background map texture layout
    DrawTexture(bg, (int)bgX, (int)bgY, WHITE);

    for (size_t i = 0; i < spacePenguins.size(); i++)
    {
      Penguin &p = spacePenguins[i];

      const float originalX = p.x;
      const float originalY = p.y;

      // Offset their positions dynamically based on the camera position
      p.x = originalX + bgX;
      p.y = originalY + bgY;

      // Render sprite onto monitor viewports
      p.display();

      // Keep internal tracking loops perfectly pristine
      p.x = originalX;
      p.y = originalY;
    }

    if (gameState == "PLAY")
    {
      // Render drifting dead ships behind active ships
      for (Spaceship *ds : deadShips)
      {
        ds->display();
      }

      for (Spaceship *ship : allShips)
      {
        ship->display();
      }

      // Update and draw weapons projectiles
      for (int i = projectiles.size() - 1; i >= 0; i--)
      {
        Projectile *p = projectiles[i];

        p->update();
        p->display();

        if (p->isDead)
        {
          delete p;
          projectiles.erase(projectiles.begin() + i);
        }
      }

      for (Alien *alien : allAliens)
      {
        alien->display();
      }

      // Remove dead aliens to clear vector memory layout
      for (int i = allAliens.size() - 1; i >= 0; i--)
      {
        if (allAliens[i]->isDead)
        {
          delete allAliens[i];                    // Delete the pointer's memory on the heap
          allAliens.erase(allAliens.begin() + i); // Clear the row slot out of the vector
        }
      }

      DrawText(TextFormat("SCORE: %d", score), 20, 20, 20, Color{255, 220, 40, 255});

      std::string hpShields = "FLEET INTEGRITY: ";
      for (int h = 0; h < fleetHealth; h++)
      {
        hpShields += "\u2665"; // Appends actual Unicode hearts to the string
      }

      // Measure the exact text boundary utilising the custom font so it stays perfectly aligned
      constexpr float fontSize = 20.0f;
      constexpr float fontSpacing = 1.0f;
      const Vector2 textSize = MeasureTextEx(unicodeFont, hpShields.c_str(), fontSize, fontSpacing);

      // Pin the position cleanly to the top right corner
      const Vector2 hpPosition = {
          (float)virtualWidth - textSize.x - 20.0f, // X Coordinate
          20.0f                                     // Y Coordinate
      };

      // Draw using DrawTextEx to support the heart glyphs
      DrawTextEx(unicodeFont, hpShields.c_str(), hpPosition, fontSize, fontSpacing, Color{255, 70, 70, 255});
    }
    if (gameState == "START")
    {
      constexpr unsigned boxWidth = 560;
      constexpr unsigned boxHeight = 420;
      const unsigned boxX = virtualWidth / 2 - boxWidth / 2;
      const unsigned boxY = virtualHeight / 2 - boxHeight / 2;

      DrawRectangle(boxX, boxY, boxWidth, boxHeight, Color{15, 15, 35, 220});
      DrawRectangleLines(boxX + 10, boxY + 10, boxWidth - 20, boxHeight - 20, Color{0, 180, 255, 100});

      const char *titleText = "GRAND TOUR: COLD FRONT";
      float titleSize = 36.0f;
      float titleSpacing = 2.0f;

      const Vector2 titleMetrics = MeasureTextEx(titleFont, titleText, titleSize, titleSpacing);
      DrawTextEx(titleFont, titleText,
                 {(float)virtualWidth / 2.0f - titleMetrics.x / 2.0f, (float)(boxY + 40)},
                 titleSize, titleSpacing, CYAN);

      const float startY = boxY + 120.0f;
      constexpr float lineSpacing = 35;
      constexpr float fontSpacing = 1.0f;
      constexpr float fs = 18.0f;

      static const char *const lines[7] = {
          "\u2022 W / S / A / D   :  Move Spaceship Fleet",
          "\u2022 C / V / B       :  Flap Penguin Arms",
          "\u2022 Z KEY           :  Shoot Fireball (Slow Cooldown)",
          "\u2022 X KEY           :  Shoot Iceball (Freezes Aliens)",
          "Lowercase and uppercase keys are supported.",
          "\u00a9 Ethan Graham. All rights reserved.",
          "See LICENSE.txt in the game directory for the full terms."};

      const Color lineColours[] = {
          GREY2, GREY2,
          ORANGE,
          SKYBLUE,
          WHITE, WHITE, WHITE};

      std::array<Vector2, std::size(lines)>
          lineSize;
      for (size_t i = 0; i < std::size(lines); i++)
      {
        lineSize[i] = MeasureTextEx(unicodeFont, lines[i], fs, fontSpacing);
        DrawTextEx(unicodeFont, lines[i], {lineX(lineSize[i]), startY + lineSpacing * i}, fs, fontSpacing, lineColours[i]);
      }

      const char *promptText = "Press 'R' to launch the fleet";
      constexpr unsigned promptSize = 20;

      // Use GetTime() to calculate absolute alpha pulse regardless of framerate
      const float pulse = 150.0f + sinf(GetTime() * 6.0f) * 105.0f;
      const unsigned char textAlpha = static_cast<unsigned char>(pulse);

      // Measure width vector to force the top-left anchor point into a center layout
      const unsigned promptWidth = MeasureText(promptText, promptSize);
      const unsigned promptX = (virtualWidth / 2) - (promptWidth / 2);
      const unsigned promptY = boxY + boxHeight - 50; // Placed neatly near the bottom of the menu card

      DrawText(promptText, promptX, promptY, promptSize, Color{0, 255, 150, textAlpha});
    }
    else if (gameState == "GAMEOVER")
    {
      // Draw Match-Sized Translucent Menu Card Box
      constexpr unsigned boxWidth = 560;
      constexpr unsigned boxHeight = 420;
      const int boxX = virtualWidth / 2 - boxWidth / 2;
      const int boxY = virtualHeight / 2 - boxHeight / 2;

      DrawRectangle(boxX, boxY, boxWidth, boxHeight, Color{20, 10, 15, 220});
      DrawRectangleLines(boxX + 10, boxY + 10, boxWidth - 20, boxHeight - 20, Color{255, 40, 40, 100});

      // High-Res Centered Game Over Title
      const char *gameOverText = "FLEET DESTROYED";
      constexpr float titleSize = 36.0f;
      constexpr float titleSpacing = 2.0f;

      const Vector2 titleMetrics = MeasureTextEx(titleFont, gameOverText, titleSize, titleSpacing);
      const Vector2 titlePos = {
          (float)virtualWidth / 2.0f - titleMetrics.x / 2.0f,
          (float)(boxY + 35)};
      DrawTextEx(titleFont, gameOverText, titlePos, titleSize, titleSpacing, RED);

      // --- Crisp High-Res Score Readouts ---
      const char *scoreText = TextFormat("Final Score: %d", score);
      const char *highScoreText = TextFormat("Personal Best: %d", highscore);
      float scoreSize = 22.0f;
      float scoreSpacing = 1.0f;

      // Draw Final Score
      const Vector2 scoreMetrics = MeasureTextEx(unicodeFont, scoreText, scoreSize, scoreSpacing);
      const Vector2 scorePos = {
          (float)virtualWidth / 2.0f - scoreMetrics.x / 2.0f,
          (float)(boxY + 115)};
      DrawTextEx(unicodeFont, scoreText, scorePos, scoreSize, scoreSpacing, WHITE);

      // Draw High Score
      const Vector2 hiScoreMetrics = MeasureTextEx(unicodeFont, highScoreText, scoreSize, scoreSpacing);
      const Vector2 hiScorePos = {
          (float)virtualWidth / 2.0f - hiScoreMetrics.x / 2.0f,
          (float)(boxY + 145)};

      Color hiColor = (score >= highscore) ? Color{255, 215, 0, 255} : Color{180, 180, 180, 255};
      DrawTextEx(unicodeFont, highScoreText, hiScorePos, scoreSize, scoreSpacing, hiColor);

      // --- Integrated Legal Information Section ---
      constexpr float legalSize = 20.0f;
      constexpr float legalSpacing = 1.0f;
      Color legalColor = Color{170, 170, 170, 255}; // Slightly brighter clean gray

      const char *legalLines[] = {
          "Grand Tour Cold Front | Legal Info | Creator: Ethan Graham",
          "Code License: Custom Source-Available",
          "In-Game Media: Switch screenshots (non-commercial use)",
          "Fonts & Libraries: Covered under distributor EULAs",
          "Full licensing terms available in LICENSE.txt"};

      // Loop through and draw each legal line centered dynamically
      float startLegalY = boxY + 195.0f;
      for (int i = 0; i < 5; i++)
      {
        Vector2 legalMetrics = MeasureTextEx(unicodeFont, legalLines[i], legalSize, legalSpacing);
        Vector2 legalPos = {
            (float)virtualWidth / 2.0f - legalMetrics.x / 2.0f,
            startLegalY + (i * 18.0f) // 18 pixels of vertical padding per line
        };
        DrawTextEx(unicodeFont, legalLines[i], legalPos, legalSize, legalSpacing, legalColor);
      }

      // Smooth Alpha Pulsing Restart Prompt
      const char *promptText = "Press 'R' to restart the game";
      constexpr float promptSize = 20.0f;
      constexpr float promptSpacing = 1.0f;

      const float pulse = 150.0f + sinf(GetTime() * 6.0f) * 105.0f;
      const unsigned char textAlpha = static_cast<unsigned char>(pulse);

      const Vector2 promptMetrics = MeasureTextEx(unicodeFont, promptText, promptSize, promptSpacing);
      const Vector2 promptPos = {
          (float)virtualWidth / 2.0f - promptMetrics.x / 2.0f,
          (float)(boxY + boxHeight - 45)};
      DrawTextEx(unicodeFont, promptText, promptPos, promptSize, promptSpacing, Color{0, 255, 150, textAlpha});
    }

    EndTextureMode(); // Close out internal rendering

    // Hardware Scaling Step: Draw the target canvas scaled onto the real monitor screen
    BeginDrawing();
    ClearBackground(BLACK); // Clear letterboxing bars out to clean black

    UpdateViewportScale(); // Dynamically recalculate dimensions for window scale changes
    DrawTexturePro(targetCanvas.texture, sourceRec, destRec, {0, 0}, 0.0f, WHITE);

    EndDrawing();
  }

  // --- Cleanup Phase ---
  for (Spaceship *ship : allShips)
  {
    delete ship;
  }
  allShips.clear();

  for (Alien *alien : allAliens)
  {
    delete alien;
  }
  allAliens.clear();

  delete mainPenguin;
  delete redPenguin;
  delete greenPenguin;

  for (Texture2D tex : loadedBgs)
  {
    UnloadTexture(tex);
  }
  UnloadFont(unicodeFont);
  UnloadFont(titleFont);
  UnloadViewport();

  CloseWindow();
  return 0;
}