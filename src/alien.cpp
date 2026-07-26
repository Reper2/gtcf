// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1

#include <string>
#include <vector>
#include <limits>

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#include "headers/alien.h"
#include "headers/colours.h"
#include "headers/globals.h"

float Scout::baseSpeed = 0.5f;
float Charger::baseSpeed = 1.2f;
float Swerver::baseSpeed = 1.0f;

Alien::Alien(const float startX, const float startY) noexcept
    : x(startX), y(startY)
{
  wobbleOffset = randomFloat(0.0f, PI * 2.0f);
}

Alien::~Alien() noexcept
{
  // Left completely blank, but its physical presence fixes the vtable!
}

void Alien::takeDamage(const unsigned amount) noexcept
{
  this->health -= amount;
  if (this->health <= 0)
  {
    this->isDead = true;
  }
}

void Alien::drawFreezeOverlay() const noexcept
{
  if (state == "FROZEN")
  {
    const Rectangle iceBlock = {0.0f - (46.0f / 2.0f), 0.0f - (40.0f / 2.0f), 46.0f, 40.0f};
    DrawRectangleRounded(iceBlock, 0.3f, 4, iceFill);
    DrawRectangleRoundedLines(iceBlock, 0.3f, 4, 2.0f, iceStroke);

    Vector2 glarePoints[] = {
        {-15.0f, 10.0f}, {-5.0f, -15.0f}, {0.0f, -15.0f}, {-10.0f, 10.0f}};
    DrawTriangleFan(glarePoints, 4, glareColour);
  }
}

// Helper utility to find the closest target ship
Spaceship *findClosestTarget(const float x, const float y, const std::vector<Spaceship *> &ships) noexcept
{
  float closestDistance = std::numeric_limits<float>::max();
  Spaceship *target = nullptr;
  for (Spaceship *ship : ships)
  {
    float distance = Vector2DistanceSqr({x, y}, {ship->x, ship->y});
    if (distance < closestDistance)
    {
      closestDistance = distance;
      target = ship;
    }
  }
  return target;
}

Scout::Scout(const float startX, const float startY) noexcept
    : Alien(startX, startY)
{
  health = 1;
}

void Scout::update(const std::vector<Spaceship *> &ships) noexcept
{
  if (state == "FROZEN")
  {
    if (GetTime() * 1000 - freezeTimer >= freezeDuration)
    {
      state = "NORMAL";
    }
    return;
  }
  Spaceship *target = findClosestTarget(x, y, ships);
  if (target != nullptr)
  {
    float angle = atan2f(target->y - y, target->x - x);
    x += cosf(angle) * baseSpeed * 0.4f;
    y += sinf(angle) * baseSpeed * 0.4f;
  }
}

void Scout::display() const noexcept
{
  rlPushMatrix();
  rlTranslatef(x, y, 0);
  DrawEllipse(0.0f, 0.0f, 17.5f, 12.5f, alienColours[0][0]);
  DrawCircle(-8.0f, -4.0f, 6.0f, alienColours[0][1]);
  DrawCircle(8.0f, -4.0f, 6.0f, alienColours[0][1]);
  drawFreezeOverlay();
  rlPopMatrix();
}

Charger::Charger(const float startX, const float startY) noexcept
    : Alien(startX, startY)
{
  health = 1;
}

void Charger::update(const std::vector<Spaceship *> &ships) noexcept
{
  if (state == "FROZEN")
  {
    if (GetTime() * 1000 - freezeTimer >= freezeDuration)
    {
      state = "NORMAL";
    }
    return;
  }
  Spaceship *target = findClosestTarget(x, y, ships);
  if (target != nullptr)
  {
    float angle = atan2f(target->y - this->y, target->x - this->x);
    x += cosf(angle) * baseSpeed * 0.6f;
    y += sinf(angle) * baseSpeed * 0.6f;
  }
}

void Charger::display() const noexcept
{
  rlPushMatrix();
  rlTranslatef(x, y, 0);

  // Processing coordinates translated: triangle(-20, -15, 20, 0, -20, 15)
  constexpr Vector2 v1 = {-20.0f, -15.0f};
  constexpr Vector2 v2 = {-20.0f, 15.0f};
  constexpr Vector2 v3 = {20.0f, 0.0f};
  DrawTriangle(v1, v2, v3, RED); // Angry red arrowhead

  // White engine strip matching rect(-5, 0, 10, 4)
  DrawRectangle(-5, -2, 10, 4, WHITE);

  drawFreezeOverlay();
  rlPopMatrix();
}

Swerver::Swerver(const float startX, const float startY) noexcept
    : Alien(startX, startY)
{
  health = 2;
}

void Swerver::update(const std::vector<Spaceship *> &ships) noexcept
{
  if (state == "FROZEN")
  {
    if (GetTime() * 1000 - freezeTimer >= freezeDuration)
    {
      state = "NORMAL";
    }
    return;
  }
  Spaceship *target = findClosestTarget(x, y, ships);
  if (target != nullptr)
  {
    const float angle = atan2f(target->y - this->y, target->x - this->x);
    x += cosf(angle) * baseSpeed * 0.5f;
    y += (sinf(angle) * baseSpeed * 0.5f + sinf(GetTime() * 6.0f + wobbleOffset) * 2.0f);
  }
}

void Swerver::display() const noexcept
{
  rlPushMatrix();
  rlTranslatef(x, y, 0);

  // Yellow armored beetle body: rect(0, 0, 36, 30, 6)
  constexpr Rectangle beetleShell = {-18.0f, -15.0f, 36.0f, 30.0f};
  DrawRectangleRounded(beetleShell, 0.3f, 4, Color{230, 190, 20, 255});

  // Center split line shell detail: rect(0, 0, 4, 30)
  DrawRectangle(-2, -15, 4, 30, Color{30, 30, 30, 255});

  drawFreezeOverlay();
  rlPopMatrix();
}

void spawnAlienWave(const unsigned count, const std::vector<Spaceship *> &ships)
{
  Scout::baseSpeed += 0.01;
  Charger::baseSpeed += 0.001;
  Swerver::baseSpeed += 0.005;

  for (int i = 0; i < count; i++)
  {
    float validX = 0, validY = 0;
    bool safeSpawnFound = false;

    while (!safeSpawnFound)
    {
      validX = randomFloat(50, virtualWidth - 50);
      validY = randomFloat(50, virtualHeight - 50);

      bool tooCloseToAnyShip = false;
      for (Spaceship *ship : ships)
      {
        if (Vector2DistanceSqr({validX, validY}, {ship->x, ship->y}) < (300 * 300))
        {
          tooCloseToAnyShip = true;
          break;
        }
      }
      if (!tooCloseToAnyShip)
      {
        safeSpawnFound = true;
      }
    }

    const unsigned variant = static_cast<unsigned>(randomFloat(0.0f, 3.0f));
    if (variant == 0)
    {
      allAliens.push_back(new Scout(validX, validY));
    }
    else if (variant == 1)
    {
      allAliens.push_back(new Charger(validX, validY));
    }
    else
    {
      allAliens.push_back(new Swerver(validX, validY));
    }
  }
}