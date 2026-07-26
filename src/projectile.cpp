// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1


#include <vector>
#include <array>
#include <algorithm>
#include <cmath>

#include <raylib.h>
#include "rlgl.h"
#include <raymath.h>

#include "headers/globals.h"
#include "headers/projectile.h"
#include "headers/spaceship.h"
#include "headers/alien.h"
#include "headers/colours.h"

float Projectile::lastDamageTime = 0.0f;
float Projectile::damageCooldown = 1000.0f;

Projectile::Projectile(float startX, float startY, float targetX, float targetY, const std::string &type)
    : x(startX), y(startY), element(type)
{
  birthTime = GetTime() * 1000; // Store the creation time in milliseconds

  // Calculate the angle toward the mouse cursor position
  const float angle = atan2(targetY - startY, targetX - startX);

  // Break down the speed into x and y components based on the angle
  vertexX = std::cos(angle) * speed;
  vertexY = std::sin(angle) * speed;
}

void Projectile::update() noexcept
{
  x += vertexX;
  y += vertexY;

  // Update visual rotation
  rotationAngle += rotationSpeed;

  // Precise Off-Screen Bounds for Virtual Width (1600) x Virtual Height (900)
  constexpr float padding = 50.0f; // Minimal offset beyond the visible border

  const bool xOutOfBounds = (x < -padding) || (x > (float)virtualWidth + padding);
  const bool yOutOfBounds = (y < -padding) || (y > (float)virtualHeight + padding);

  // Instantly mark dead when leaving play area (no artificial timer required)
  isDead = isDead || xOutOfBounds || yOutOfBounds;
}

void Projectile::display() const noexcept
{
  rlPushMatrix();
  rlTranslatef(x, y, 0);
  rlRotatef(rotationAngle * RAD2DEG, 0, 0, 1.0f); // Match the spin matrix

  if (element == "fire")
  {
    // Outer Plasma Flares (Diameters: 26x22, 25x18 -> Radii: 13x11, 12.5x9)
    DrawEllipse(0, 0, 13.0f, 11.0f, fireballColours[0]);
    rlPushMatrix();
    rlRotatef(60.0f, 0, 0, 1.0f); // THIRD_PI rotation
    DrawEllipse(0, 0, 12.5f, 9.0f, fireballColours[0]);
    rlPopMatrix();

    // Main Fireball Outer Body (Diameter: 20 -> Radius: 10)
    DrawCircle(0, 0, 10.0f, fireballColours[1]);

    // Dynamic Flame Crown Accent (Offset coordinates mapped directly!)
    // circle(3, -2, 14) -> Center: (3, -2), Radius: 7
    DrawCircle(3, -2, 7.0f, fireballColours[2]);
    // ellipse(-2, 3, 16, 12) -> Center: (-2, 3), Radii: 8x6
    DrawEllipse(-2, 3, 8.0f, 6.0f, fireballColours[2]);

    // Inner Core (circle(1, -1, 10) -> Center: (1, -1), Radius: 5)
    DrawCircle(1, -1, 5.0f, fireballColours[3]);

    // Fireball Core Spark (circle(0, 0, 5) -> Center: (0, 0), Radius: 2.5)
    DrawCircle(0, 0, 2.5f, fireballColours[4]);
  }
  else if (element == "ice")
  {
    // Outer Icy Vapour Layer (Diameters: 26x22, 25x18 -> Radii: 13x11, 12.5x9)
    DrawEllipse(0, 0, 13.0f, 11.0f, iceballColours[0]);
    rlPushMatrix();
    rlRotatef(60.0f, 0, 0, 1.0f);
    DrawEllipse(0, 0, 12.5f, 9.0f, iceballColours[0]);
    rlPopMatrix();

    // Main Outer Layer (Diameter: 20 -> Radius: 10)
    DrawCircle(0, 0, 10.0f, iceballColours[1]);

    // Inner Layer of Icy Shards (Offset coordinates mapped directly!)
    // circle(-3, 2, 14) -> Center: (-3, 2), Radius: 7
    DrawCircle(-3, 2, 7.0f, iceballColours[2]);
    // ellipse(2, -3, 16, 12) -> Center: (2, -3), Radii: 8x6
    DrawEllipse(2, -3, 8.0f, 6.0f, iceballColours[2]);

    // 4. Cryogenic Core (circle(-1, 1, 10) -> Center: (-1, 1), Radius: 5)
    DrawCircle(-1, 1, 5.0f, iceballColours[3]);

    // 5. Core 0k Flash (circle(0, 0, 5) -> Center: (0, 0), Radius: 2.5)
    DrawCircle(0, 0, 2.5f, iceballColours[4]);
  }

  rlPopMatrix();
}

void Projectile::processGlobalCollisions()
{
  // 1. Projectile hits Alien
  for (std::size_t i = projectiles.size(); i > 0; --i)
  {
    Projectile *proj = projectiles[i - 1];
    if (proj == nullptr || proj->isDead) continue;

    for (std::size_t j = allAliens.size(); j > 0; --j)
    {
      Alien *alien = allAliens[j - 1];
      if (alien == nullptr || alien->isDead) continue;

      const Vector2 projPos = {proj->x, proj->y};
      const Vector2 alienPos = {alien->x, alien->y};
      const float distance = Vector2DistanceSqr(projPos, alienPos);

      if (distance < (38.0f * 38.0f))
      {
        proj->isDead = true; 
        if (proj->element == "ice")
        {
          alien->state = "FROZEN";
          alien->freezeTimer = GetTime() * 1000.0f;
        }
        else if (proj->element == "fire")
        {
          if (alien->state == "FROZEN")
          {
            score += 100;
            alien->isDead = true;
          }
          else
          {
            --alien->health;
            proj->isDead = true;
            if (alien->health <= 0)
            {
              score += 250;
              alien->isDead = true;
            }
          }
        }
        break; // Exit alien loop once projectile hits
      }
    }
  }

  // Alien hits Spaceship
  if ((GetTime() * 1000.0f) - lastDamageTime > damageCooldown)
  {
    for (std::size_t i = allAliens.size(); i > 0; --i)
    {
      Alien *alien = allAliens[i - 1];
      if (alien == nullptr || alien->isDead) continue;

      const Vector2 alienPos = {alien->x, alien->y};
      bool alienDestroyed = false;

      // Safely loop backward through active ships
      for (std::size_t s = allShips.size(); s > 0; --s)
      {
        const std::size_t shipIndex = s - 1;
        Spaceship *ship = allShips[shipIndex];
        if (ship == nullptr) continue;

        const Vector2 shipPos = {ship->x, ship->y};
        const float distance = Vector2DistanceSqr(shipPos, alienPos);

        if (distance < (45.0f * 45.0f) || distance < (38.0f * 38.0f))
        {
          if (alien->state == "FROZEN")
          {
            score += 100;
            alien->isDead = true;
            alienDestroyed = true;
          }
          else
          {
            if (fleetHealth <= 3)
            {
              if (ship == redShip)    { redShip = nullptr; redPenguin = nullptr; }
              if (ship == mainShip)   { mainShip = nullptr; mainPenguin = nullptr; }
              if (ship == greenShip)  { greenShip = nullptr; greenPenguin = nullptr; }

              ship->isDrifting = true;
              deadShips.push_back(ship);

              // Safely remove ship from vector without breaking the loop index
              allShips.erase(allShips.begin() + shipIndex);
            }

            --fleetHealth;
            lastDamageTime = GetTime() * 1000.0f;
            score = (score >= 50) ? (score - 50) : 0;
            alienDestroyed = true;
          }
          break; // Exit ship loop
        }
      }

      if (alienDestroyed)
      {
        alien->isDead = true;
      }
    }
  }
}