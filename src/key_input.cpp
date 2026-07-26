// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1


#include <raylib.h>
#include <string>
#include <cmath>
#include <vector>
#include "headers/key_input.h"
#include "headers/globals.h"
#include "headers/spaceship.h"
#include "headers/projectile.h"

bool resolveKeys(const unsigned index, const std::string &keys)
{
  if (index >= keys.length())
  {
    return false;
  }
  const char lowerChar = std::tolower(keys[index]);
  return keysHeld[static_cast<unsigned char>(lowerChar)];
}

void shootProjectile(const std::string &keys, const std::array<unsigned, 2>index)
{
  // Weapon Projectile Engine Layer
  const bool wantFire = resolveKeys(index[0], keys); // 'z'
  const bool wantIce = resolveKeys(index[1], keys);  // 'x'

  if (wantFire || wantIce)
  {
    bool canShoot = false;
    std::string type = "";
    const int currentMillis = GetTime() * 1000;

    if (wantFire && (currentMillis - lastFireTime > fireDelay))
    {
      type = "fire";
      canShoot = true;
      lastFireTime = currentMillis;
    }
    else if (wantIce && (currentMillis - lastIceTime > iceDelay))
    {
      type = "ice";
      canShoot = true;
      lastIceTime = currentMillis;
    }

    if (canShoot)
    {
      for (Spaceship *s : allShips)
      {
        // Offset projectile start to spawn right at each ship's weapon nose cone
        const float noseX = s->x + (110.0f * s->scaleFact);
        const float noseY = s->y;

        projectiles.push_back(new Projectile(noseX, noseY, (float)GetMouseX(), (float)GetMouseY(), type));
      }
    }
  }
}

void handleMainInp(Spaceship* const mainShip, const std::string &keys)
{
  if (gameState != "PLAY" || mainShip == nullptr)
  {
    return;
  }

  float dx = 0.0f;
  float dy = 0.0f;
  constexpr float baseSpeed = 4.0f;

  // Calculate intended directional changes scaled by our global speed delta
  if (resolveKeys(0, keys))
  {
    dy -= baseSpeed; // Up
  }
  if (resolveKeys(1, keys))
  {
    dy += baseSpeed; // Down
  }
  if (resolveKeys(2, keys))
  {
    dx -= baseSpeed; // Left
  }
  if (resolveKeys(3, keys))
  {
    dx += baseSpeed; // Right
  }

  // Map Background Scrolling Boundaries matching your Processing limits
  // Checks if the world texture has remaining room to scroll before shifting entities
  bool canScrollX = (dx < 0.0f && bgX < 0.0f) ||
                    (dx > 0.0f && bgX > ((float)virtualWidth - bg.width));

  bool canScrollY = (dy < 0.0f && bgY < 0.0f) ||
                    (dy > 0.0f && bgY > ((float)virtualHeight - bg.height));

  // Process X Movement / Scrolling
  if (canScrollX)
  {
    bgX -= dx; // Move the world backward to scroll forward
  }
  else if (dx != 0.0f)
  {
    // Check screen edge boundaries using the flagship's size modifier
    const float shipSizeX = 100.0f * mainShip->scaleFact;
    if (mainShip->x + dx > shipSizeX / 2.0f && mainShip->x + dx < (float)virtualWidth - shipSizeX / 2.0f)
    {
      // Apply the displacement uniformly across all 3 active ships
      for (Spaceship *ship : allShips)
      {
        ship->x += dx;
        if (ship->pilot != nullptr)
        {
          ship->pilot->x += dx;
        }
      }
    }
  }

  // Process Y Movement / Scrolling
  if (canScrollY)
  {
    bgY -= dy; // Move the world vertical grid plane
  }
  else if (dy != 0.0f)
  {
    const float shipSizeY = 180.0f * mainShip->scaleFact;
    if (mainShip->y + dy > shipSizeY / 2.0f && mainShip->y + dy < (float)virtualHeight - shipSizeY / 2.0f)
    {
      // Apply vertical displacement uniformly across all 3 active ships
      for (Spaceship *ship : allShips)
      {
        ship->y += dy;
        if (ship->pilot != nullptr)
        {
          ship->pilot->y += dy;
        }
      }
    }
  }

  shootProjectile(keys, {4, 5});
}

void handlePilotInp(Penguin* const pilot, const std::string &keys)
{
  if (pilot == nullptr)
  {
    return;
  }
  pilot->flap = resolveKeys(0, keys);

  shootProjectile(keys, {1, 2});
}