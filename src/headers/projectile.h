// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1


#pragma once
#include <raylib.h>
#include <string>

class Alien; // Forward declaration

class Projectile
{
public:
  float x, y;
  // Velocity components calculated at launch
  float vertexX, vertexY;
  float speed = 2;
  // "fire" or "ice"
  std::string element;
  float rotationAngle = 0;
  float rotationSpeed = 0.22;
  bool isDead = false;
  int birthTime; // Time of creation in milliseconds
  static float lastDamageTime;
  static float damageCooldown;

  Projectile(float startX, float startY, float targetX, float targetY, const std::string &type);
  void update() noexcept;
  void display() const noexcept;
  static void processGlobalCollisions();
};