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

#include <raylib.h>
#include <rlgl.h>

#include "headers/colours.h"
#include "headers/globals.h"

#include "headers/penguin.h"

Penguin::Penguin(float x, float y, float size, Color colour) noexcept
    : x(x), y(y), colour(colour), alpha(255), slideAngle(0.0f), flap(false)
{
  const float minDim = (virtualWidth < virtualHeight) ? (float)virtualWidth : (float)virtualHeight;
  this->scaleFact = size * (minDim / 500.0f);

  arms = {Arm(-1), Arm(1)};
  feet = {Feet(-1), Feet(1)};
}

Penguin::Penguin(float x, float y, float size, Color colour, int alphaValue) noexcept
    : Penguin(x, y, size, colour)
{
  this->alpha = alphaValue;
}

void Penguin::update() noexcept
{
  if (flap)
  {
    for (Arm &arm : arms)
    {
      arm.animate();
    }
  }
}

void Penguin::display() noexcept
{
  rlPushMatrix();
  rlTranslatef(x, y, 0);
  rlRotatef(slideAngle * RAD2DEG, 0, 0, 1.0f);
  rlScalef(scaleFact, scaleFact, 1.0f);

  for (const Arm &arm : arms)
  {
    arm.display(colour, alpha);
  }

  displayBody();
  head.display(colour, alpha);

  for (const Feet &foot : feet)
  {
    foot.display(alpha);
  }

  rlPopMatrix();
}

void Penguin::displayBody() noexcept
{
  // Processing: ellipse(0, -5, 100*1.8, 180) -> Diameter 180x180 -> Raylib Radius 90x90
  Color bodyColour = colour;
  bodyColour.a = alpha;
  DrawEllipse(0, -5, 90.0f, 90.0f, bodyColour);

  // Processing: ellipse(0, 0, 100*1.8, 179) -> Diameter 180x179 -> Raylib Radius 90x89.5
  Color whiteColour = WHITE;
  whiteColour.a = alpha;
  DrawEllipse(0, 0, 90.0f, 179.0f / 2.0f, whiteColour);
}

// Arm implementations
Penguin::Arm::Arm(int side) noexcept
    : side(side), angle(((side == 1) ? 45.0f : -45.0f)), velocity(2.6f) {}

void Penguin::Arm::animate() noexcept
{
  const float minA = (side == 1) ? 0.0f : -45.0f;
  const float maxA = (side == 1) ? 45.0f : 0.0f;

  if (angle > maxA || angle < minA)
  {
    velocity *= -1.0f;
  }
  angle += velocity;
}

void Penguin::Arm::display(Color bodyColour, int alpha) const noexcept
{
  rlPushMatrix();
  rlTranslatef(side * 100.0f, -30.0f, 0);
  rlRotatef(angle, 0, 0, 1.0f); // Processing handles angles via radians natively, but custom wrappers can use degrees

  // Processing: ellipse(0, 0, 150, 28) -> Radius 75x14
  Color armColour = bodyColour;
  armColour.a = alpha;
  DrawEllipse(0, 0, 75.0f, 14.0f, armColour);

  // Processing: ellipse(0, 6, 150, 14) -> Radius 75x7
  Color whiteColour = WHITE;
  whiteColour.a = alpha;
  DrawEllipse(0, 6.0f, 75.0f, 7.0f, whiteColour);

  rlPopMatrix();
}

// Feet implementations
Penguin::Feet::Feet(int side) noexcept
    : side(side), footX(40.0f * side) {}

void Penguin::Feet::display(int alpha) const noexcept
{
  const Color yellowFeet = {255, 200, 0, (unsigned char)alpha};
  const Vector2 centre = {footX, 87.0f};
  // Processing: arc(footX, 87, 70, 27, 0, PI) -> Radius 35x13.5
  rlPushMatrix();
  rlTranslatef(centre.x, centre.y, 0);
  rlScalef(35.0f, 13.5f, 1.0f);
  DrawCircleSector({0, 0}, 1.0f, 0.0f, 180.0f, 24, yellowFeet);
  rlPopMatrix();
}

// Head implementation
Penguin::Head::Head() noexcept {}

void Penguin::Head::display(Color bodyColour, int alpha) noexcept
{
  rlPushMatrix();
  rlTranslatef(0, -85.0f, 0); // Move into isolated Head Context

  Color headColour = bodyColour;
  headColour.a = alpha;

  // Processing: arc(0, -2, 140, 135, PI, TWO_PI) -> Radius 70x67.5
  rlPushMatrix();
  rlTranslatef(0, -2.0f, 0);
  rlScalef(70.0f, 67.5f, 1.0f);
  DrawCircleSector({0, 0}, 1.0f, 180.0f, 360.0f, 24, headColour);
  rlPopMatrix();

  // Processing: ellipse(0, -2, 140, 30) -> Radius 70x15
  DrawEllipse(0, -2, 70.0f, 15.0f, headColour);

  constexpr float eyeSizes[] = {25.0f, 15.0f, 15.0f, 7.0f};

  const Color eyeColours[4] = {
      {255, 255, 255, (unsigned char)alpha}, // White eyeball
      {83, 195, 189, (unsigned char)alpha},  // Iris (Teal)
      {0, 0, 0, (unsigned char)alpha},       // Pupil
      {255, 255, 255, (unsigned char)alpha}  // Reflection
  };

  for (unsigned type = 0; type < 4; type++)
  {
    float offsetX = 32.0f;
    float eyeY = -35.0f;

    if (type == 1 || type == 2 || type == 3)
    {
      offsetX -= 3.0f;
    }
    if (type == 1)
    {
      offsetX += 2.0f;
    }
    if (type == 3)
    {
      offsetX -= 2.0f;
      eyeY -= 3.0f;
    }

    // Processing: circle() takes diameter, but multiplies size by 1.2 -> radius = (size * 1.2) / 2 = size * 0.6
    const float finalRadius = eyeSizes[type] * 0.6f;

    DrawCircleV({-offsetX, eyeY}, finalRadius, eyeColours[type]);
    DrawCircleV({offsetX, eyeY}, finalRadius, eyeColours[type]);
  }

  // Draw Beak Components
  displayBeak(alpha);
  rlPopMatrix(); // Pop head transformation context
}

void Penguin::Head::displayBeak(int alpha) noexcept
{
  const Color yellowBeak = {255, 255, 0, (unsigned char)alpha};

  // 1. Lower Beak Arc
  rlPushMatrix();
  rlTranslatef(0, -15.0f, 0);
  rlScalef(35.0f, 20.0f, 1.0f);
  DrawCircleSector({0, 0}, 1.0f, 0.0f, 180.0f, 24, yellowBeak);
  rlPopMatrix();

  // 2. Upper Beak Triangle
  // Vertex 0 is the highest center point, then we loop counter-clockwise around the base.
  constexpr Vector2 beakPoints[] = {
      {0.0f, -25.0f},   // Center top tip (pointing UP)
      {-28.0f, -15.0f}, // Left corner base
      {28.0f, -15.0f}   // Right corner base
  };

  DrawTriangleFan((Vector2 *)beakPoints, 3, yellowBeak);
}

void Penguin::moveX(const float dx, const float virtualWidth, float &bgX, const Texture2D bg) noexcept
{
  const float multiplier = (slideAngle != 0) ? 2.0f : 1.0f;
  const float moveStep = dx * multiplier;
  const float size = 100.0f * scaleFact;
  const bool canScroll = (moveStep < 0 && bgX < 0) || (moveStep > 0 && bgX > virtualWidth - bg.width);

  if (canScroll)
  {
    bgX -= moveStep;
  }
  else if (x + moveStep > size / 2.0f && x + moveStep < virtualWidth - size / 2.0f)
  {
    x += moveStep;
  }
}

void Penguin::moveY(const float dy, const float virtualHeight, float &bgY, const Texture2D bg) noexcept
{
  const float size = 180.0f * scaleFact;
  const bool canScroll = (dy < 0 && bgY < 0) || (dy > 0 && bgY > virtualHeight - bg.height);

  if (canScroll)
  {
    bgY -= dy;
  }
  else if (y + dy > size / 2.0f && y + dy < virtualHeight - size / 2.0f)
  {
    y += dy;
  }
}