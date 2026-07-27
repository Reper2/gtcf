// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1

#pragma once
#include <array>
#include <vector>
#include "penguin.h"

class Spaceship
{
public:
  float x;
  float y;
  float scaleFact;
  Font logoFont;
  bool isDrifting = false;

  // Spaceship palette: [0]=Hull, [1]=Fins, [2]=Engine Glow, [3]=Cockpit Trim
  std::array<Color, 4> colour;
  Penguin *pilot = nullptr; // Pointer to the pilot object

  // Pilot Layout offsets inside the cockpit
  float pilotScale = 0.2f;
  float pilotOffsetX = 5.0f;
  float pilotOffsetY = -2.0f;
  class TrailParticle
  {
  public:
    float particleX, particleY;
    float vertexX, vertexY;
    float pSize;
    float alpha = 255.0f;
    float rotation = 0.0f;
    float rotSpeed;
    int palleteLength;
    int randomIndex;
    Color particleColor;

    TrailParticle(float startX, float startY) noexcept;
    void update() noexcept;
    void display() noexcept;
    bool isDead() const noexcept;
  };

  // Vector to store active exhaust particles
  std::vector<TrailParticle> trail;

  Spaceship(float x, float y, float size, const std::array<Color, 4> &shipPallete, Penguin *pilot);

  void update();
  void display() noexcept;
  void moveX(const float dx, const float virtualWidth, float &bgX, const Texture2D bg) noexcept;
  void moveY(const float dy, const float virtualHeight, float &bgY, const Texture2D bg) noexcept;

private:
  // Display helper pipeline
  void displayTrail() noexcept;
  void displayPilot() const noexcept;
  void displayFins() const noexcept;
  void displayLogo() const noexcept;
  void displayEngines() const noexcept;
  void displayHull() const noexcept;
  void displayCockpit() const noexcept;
};