// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1


#pragma once
#include <vector>
#include <raylib.h>

class Penguin
{
public:
  // Attributes
  float x, y;
  Color colour; // Directly uses Raylib's RGBA struct
  float scaleFact;
  float slideAngle = 0.0f;
  int alpha = 255;
  bool flap = false;

  // --- NESTED CLASSES (The Blueprints) ---
  class Feet
  {
  public:
    int side;
    float footX = 40.0f;
    Feet(int side) noexcept;
    void display(int alpha) const noexcept;
  };

  class Head
  {
  public:
    class Eye
    {
    public:
      int type;
      // Native Raylib Color presets array
      Color colours[4];
      float sizes[4] = {25.0f, 15.0f, 15.0f, 7.0f};

      Eye(int type) noexcept;
      void display(int alpha) noexcept;
    };

    std::vector<Eye> eyes;
    Head() noexcept;
    void display(Color bodyColour, int alpha) noexcept;
    void displayBeak(int alpha) noexcept;
  };

  class Arm
  {
  public:
    int side;
    float angle;
    float velocity = 2.6f;

    Arm(int side) noexcept;
    void animate() noexcept;
    void display(Color bodyColour, int alpha) const noexcept;
  };

  // --- PENGUIN INSTANCE VARIABLES ---
  Head head;
  std::vector<Arm> arms;
  std::vector<Feet> feet;

  // Constructors
  Penguin(float x, float y, float size, Color colour) noexcept;
  Penguin(float x, float y, float size, Color colour, int alphaValue) noexcept;

  // Methods
  void update() noexcept;
  // void moveX(const float dx, const float virtualWidth, float &bgX, const Texture2D bg) noexcept;
  // void moveY(const float dy, const float virtualHeight, float &bgY, const Texture2D bg) noexcept;
  void display() noexcept;
  void displayBody() noexcept;
};