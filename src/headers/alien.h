// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1


#pragma once
#include <string>
#include <vector>
#include "spaceship.h"

class Alien
{
public:
  float x, y;
  virtual float GetSpeed() const = 0;
  // "Scout", "Charger", "Swerver"
  std::string type;
  // "NORMAL" or "FROZEN"
  std::string state = "NORMAL";
  int freezeTimer = 0;
  // Freezing an alien gives the player four seconds to terminate it
  int freezeDuration = 4000;
  unsigned health;
  bool isDead;
  bool isOffScreen;
  float wobbleOffset;

  Alien(const float startX, const float startY) noexcept;
  virtual ~Alien() noexcept;

  virtual std::string getType() const noexcept = 0;
  virtual void update(const std::vector<Spaceship *> &ships) noexcept = 0;
  virtual void display() const noexcept = 0;
  void takeDamage(const unsigned amount) noexcept;
  void drawFreezeOverlay() const noexcept;
};

// Concrete Subclass: Scout
class Scout : public Alien
{
public:
  Scout(const float startX, const float startY) noexcept;
  std::string getType() const noexcept override { return "Scout"; }
  void update(const std::vector<Spaceship *> &ships) noexcept override;
  void display() const noexcept override;

  static float baseSpeed;
  float GetSpeed() const noexcept override { return baseSpeed; }
};

// Concrete Subclass: Charger
class Charger : public Alien
{
public:
  Charger(const float startX, const float startY) noexcept;
  std::string getType() const noexcept override { return "Charger"; }
  void update(const std::vector<Spaceship *> &ships) noexcept override;
  void display() const noexcept override;

  static float baseSpeed;
  float GetSpeed() const noexcept override { return baseSpeed; }
};

// Concrete Subclass: Swerver
class Swerver : public Alien
{
public:
  Swerver(const float startX, const float startY) noexcept;
  std::string getType() const noexcept override { return "Swerver"; }
  void update(const std::vector<Spaceship *> &ships) noexcept override;
  void display() const noexcept override;

  static float baseSpeed;
  float GetSpeed() const noexcept override { return baseSpeed; }
};

Spaceship *findClosestTarget(const float x, const float y, const std::vector<Spaceship *> &ships) noexcept;
void spawnAlienWave(const unsigned count, const std::vector<Spaceship *> &ships);