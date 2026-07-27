// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1

#include <array>
#include <cmath>
#include <string>
#include <vector>

#include "headers/globals.h"
#include "headers/projectile.h"
#include "headers/spaceship.h"

#include "headers/key_input.h"

bool resolveKeys(const unsigned index, const std::string &keys)
{
  if (index >= keys.length())
  {
    return false;
  }
  const char lowerChar = std::tolower(keys[index]);
  return keysHeld[static_cast<unsigned char>(lowerChar)];
}

void shootProjectile(const std::string &keys, const std::array<unsigned, 2> index)
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
      // Determine target coordinates from Mouse or Touch
      float targetX = 0.0f;
      float targetY = 0.0f;

      // Map mouse to virtual coordinates
      targetX = ((GetMouseX() - destRec.x) / destRec.width) * virtualWidth;
      targetY = ((GetMouseY() - destRec.y) / destRec.height) * virtualHeight;

      if (GetTouchPointCount() > 0)
      {
        Vector2 touchPos = GetTouchPosition(0);
        // Map mobile touch position correctly across letterboxing and zoom scales
        targetX = ((touchPos.x - destRec.x) / destRec.width) * virtualWidth;
        targetY = ((touchPos.y - destRec.y) / destRec.height) * virtualHeight;
      }

      for (Spaceship *s : allShips)
      {
        if (s == nullptr)
          continue;
        const float noseX = s->x + (110.0f * s->scaleFact);
        const float noseY = s->y;

        projectiles.push_back(new Projectile(noseX, noseY, targetX, targetY, type));
      }
    }
  }
}

void handleMainInp(Spaceship *const mainShip, const std::string &keys)
{
  if (gameState != "PLAY" || mainShip == nullptr)
  {
    return;
  }

  float dx = 0.0f;
  float dy = 0.0f;
  constexpr float baseSpeed = 4.0f;

  if (resolveKeys(0, keys))
    dy -= baseSpeed; // Up
  if (resolveKeys(1, keys))
    dy += baseSpeed; // Down
  if (resolveKeys(2, keys))
    dx -= baseSpeed; // Left
  if (resolveKeys(3, keys))
    dx += baseSpeed; // Right

  bool canScrollX = (dx < 0.0f && bgX < 0.0f) || (dx > 0.0f && bgX > ((float)virtualWidth - bg.width));
  bool canScrollY = (dy < 0.0f && bgY < 0.0f) || (dy > 0.0f && bgY > ((float)virtualHeight - bg.height));

  if (dx != 0.0f)
  {
    if (canScrollX)
    {
      bgX -= dx;
      // Shift all other ships alongside the world background scroll
      for (Spaceship *ship : allShips)
      {
        if (ship != mainShip && ship != nullptr)
        {
          ship->x += dx;
          if (ship->pilot != nullptr)
            ship->pilot->x += dx;
        }
      }
    }
    else
    {
      // Apply movement and strictly clamp all ships so none can escape the screen
      for (Spaceship *ship : allShips)
      {
        if (ship == nullptr)
          continue;
        const float shipSizeX = 250.0f * ship->scaleFact;
        ship->x = std::clamp(ship->x + dx, shipSizeX / 2.0f, (float)virtualWidth - (shipSizeX / 2.0f));
        if (ship->pilot != nullptr)
        {
          ship->pilot->x = ship->x;
        }
      }
    }
  }

  if (dy != 0.0f)
  {
    if (canScrollY)
    {
      bgY -= dy;
      for (Spaceship *ship : allShips)
      {
        if (ship != mainShip && ship != nullptr)
        {
          ship->y += dy;
          if (ship->pilot != nullptr) ship->pilot->y += dy;
        }
      }
    }
    else
    {
      // 1. Move all ships normally first
      for (Spaceship *ship : allShips)
      {
        if (ship == nullptr) continue;
        const float halfHeight = 80.0f * ship->scaleFact;
        ship->y = std::clamp(ship->y + dy, halfHeight, (float)virtualHeight - halfHeight);
      }

      // 2. Enforce minimum vertical separation from the main ship so they never overlap
      constexpr float minShipPadding = 60.0f;

      if (redShip != nullptr && mainShip != nullptr)
      {
        // Red ship is above mainShip: enforce it stays at least 'minShipPadding' above
        if (redShip->y > mainShip->y - minShipPadding)
        {
          redShip->y = mainShip->y - minShipPadding;
        }
      }

      if (greenShip != nullptr && mainShip != nullptr)
      {
        // Green ship is below mainShip: enforce it stays at least 'minShipPadding' below
        if (greenShip->y < mainShip->y + minShipPadding)
        {
          greenShip->y = mainShip->y + minShipPadding;
        }
      }

      // 3. Keep pilots attached
      for (Spaceship *ship : allShips)
      {
        if (ship != nullptr && ship->pilot != nullptr)
        {
          ship->pilot->y = ship->y;
        }
      }
    }
  }

  shootProjectile(keys, {4, 5});
}

void handlePilotInp(Penguin *const pilot, const std::string &keys)
{
  if (pilot == nullptr)
  {
    return;
  }
  pilot->flap = resolveKeys(0, keys);

  shootProjectile(keys, {1, 2});
}