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
#include <algorithm>
#include "rlgl.h"

#include "headers/spaceship.h"
#include "headers/globals.h"
#include "headers/colours.h"
#include "headers/penguin.h"

Spaceship::Spaceship(float x, float y, float size, const std::array<Color, 4> &shipPallete, Penguin *pilot)
    : x(x), y(y), colour(shipPallete), pilot(pilot)
{
  // Match original scale mapping logic exactly
  const float minDim = (virtualWidth < virtualHeight) ? (float)virtualWidth : (float)virtualHeight;
  scaleFact = size * (minDim / 500.0f);

  logoFont = LoadFontEx("resources/fonts/Helvetica.ttf", 48, nullptr, 0);

  GenTextureMipmaps(&logoFont.texture);
  SetTextureFilter(logoFont.texture, TEXTURE_FILTER_TRILINEAR);
}

void Spaceship::update()
{
  if (pilot != nullptr)
  {
    if (!isDrifting)
    {
      pilot->x = x;
      pilot->y = y;
    }
    pilot->update();
  }

  // Triple trail particle generators matching screen coordinates
  const float upperBoosterX = x - (75.0f * scaleFact);
  const float upperBoosterY = y - (20.0f * scaleFact);

  const float centerEngineX = x - (85.0f * scaleFact);
  const float centerEngineY = y;

  const float lowerBoosterX = x - (75.0f * scaleFact);
  const float lowerBoosterY = y + (20.0f * scaleFact);

  trail.push_back(TrailParticle(upperBoosterX, upperBoosterY));
  trail.push_back(TrailParticle(centerEngineX, centerEngineY));
  trail.push_back(TrailParticle(lowerBoosterX, lowerBoosterY));

  for (auto &particle : trail)
  {
    particle.update();
  }

  trail.erase(std::remove_if(trail.begin(), trail.end(),
                             [](const TrailParticle &p)
                             { return p.isDead(); }),
              trail.end());
}

void Spaceship::displayTrail() noexcept
{
  for (TrailParticle &particle : trail)
  {
    particle.display();
  }
}

void Spaceship::displayPilot() const noexcept
{
  if (pilot == nullptr)
    return;

  const float originalScale = pilot->scaleFact;
  pilot->scaleFact = scaleFact * pilotScale;

  if (!isDrifting)
  {
    pilot->x = x + (15.0f * scaleFact) + (pilotOffsetX * scaleFact);
    pilot->y = y - (18.0f * scaleFact) + (pilotOffsetY * scaleFact);
    pilot->slideAngle = (PI / 4.0f) - ((PI / 4.0f) / 1.25f);
  }
  else
  {
    // Gently scale down the pilot slightly as they drift away into deep space
    pilot->scaleFact *= 0.7f;
  }

  pilot->display();
  pilot->scaleFact = originalScale;
}

void Spaceship::displayFins() const noexcept
{
  constexpr static const Vector2 topFin[] = {{-30.0f, -60.0f}, {-75.0f, -5.0f}, {-30.0f, 0.0f}};
  constexpr static const Vector2 bottomFin[] = {{-30.0f, 0.0f}, {-75.0f, 5.0f}, {-30.0f, 60.0f}};

  DrawTriangleFan((Vector2 *)topFin, 3, colour[1]);
  DrawTriangleFan((Vector2 *)bottomFin, 3, colour[1]);
}

void Spaceship::displayLogo() const noexcept
{
  rlPushMatrix();
  rlTranslatef(-42.0f, -33.0f, 0);
  rlRotatef(90.0f, 0, 0, 1.0f);

  // 1. Logo Eyes
  auto const [ball, iris, pupil, reflect] = eyeBlueprint;
  for (unsigned i = 0; i < 2; i++)
  {
    const float s = (i * 2) - 1.0f;
    const float currentEyeX = s * ball.x;

    DrawCircleV({currentEyeX, ball.y}, ball.size / 2, colour[0]);
    DrawCircleV({s * iris.x, iris.y}, iris.size / 2, Color{83, 195, 189, 255});
    DrawCircleV({s * pupil.x, pupil.y}, pupil.size / 2, colour[0]);
    DrawCircleV({currentEyeX - 0.2f, reflect.y}, reflect.size / 2, colour[1]);
  }

  // 2. Logo Beak
  constexpr float beakOffsetY = 0.5f;
  constexpr float beakWidth = 8.05f;
  constexpr float beakHeight = 2.30f;
  constexpr float triWidthHalf = (beakWidth * 0.80f) / 2.0f;
  constexpr float triTipHeight = beakHeight * 0.50f;
  constexpr float baseY = -1.7f + beakOffsetY;

  rlPushMatrix();
  rlTranslatef(0.0f, baseY, 0.0f);
  rlScalef(beakWidth / 2.0f, beakHeight, 1.0f);
  DrawCircleSector({0.0f, 0.0f}, 1.0f, 0.0f, 180.0f, 24, colour[0]);
  rlPopMatrix();

  constexpr Vector2 beakPoints[] = {
      {0.0f, baseY - triTipHeight},
      {-triWidthHalf, baseY},
      {triWidthHalf, baseY}};
  DrawTriangleFan(beakPoints, 3, colour[0]);

  // 3. Logo Outer Ring
  DrawRingLines({0.0f, -2.0f}, (17.0f / 2.0f) - 1.0f, 17.0f / 2.0f, 0.0f, 360.0f, 36, colour[0]);

  // 4. Logo Text
  const char *logoText = "GTCF";
  constexpr float fontSize = 4.0f;
  constexpr float renderFontSize = 36.0f;
  constexpr float scaleFactorCompensate = fontSize / renderFontSize;

  const Vector2 textOrigin = {
      MeasureTextEx(logoFont, logoText, renderFontSize, 0.5f).x / 2.0f,
      MeasureTextEx(logoFont, logoText, renderFontSize, 0.5f).y / 2.0f};

  rlPushMatrix();
  rlTranslatef(0.0f, 3.0f, 0.0f);
  rlScalef(scaleFactorCompensate, scaleFactorCompensate, 1.0f);
  DrawTextPro(logoFont, logoText, {0.0f, 0.0f}, textOrigin, 0.0f, renderFontSize, 0.1f, colour[0]);
  rlPopMatrix();

  rlPopMatrix(); // Exit Logo Matrix
}

void Spaceship::displayEngines() const noexcept
{
  // Central Engine Glow
  constexpr Rectangle centerEngineBox = {-80.0f - (12.0f / 2.0f), 0.0f - (20.0f / 2.0f), 12.0f, 20.0f};
  DrawRectangleRounded(centerEngineBox, 0.2f, 4, colour[2]);

  // Dome Rocket Boosters
  constexpr float engineRadiusX = 20.0f / 2.0f;
  constexpr float engineRadiusY = 30.0f / 2.0f;
  constexpr Vector2 engineCentre = {0.0f, 0.0f};
  constexpr Color domeGrey = {140, 142, 145, 255};

  for (unsigned i = 0; i < 2; i++)
  {
    const float yOffset = (i == 0) ? -20.0f : 20.0f;

    rlPushMatrix();
    rlTranslatef(-55.0f, yOffset, 0);
    rlRotatef(90.0f, 0, 0, 1.0f);

    rlPushMatrix();
    rlScalef(engineRadiusX, engineRadiusY, 1.0f);
    DrawCircleSector(engineCentre, 1.0f, 0.0f, 180.0f, 32, domeGrey);
    rlPopMatrix();

    rlPopMatrix();
  }

  // Glowing Nozzles
  constexpr Rectangle topNozzle = {-70.0f - (4.0f / 2.0f), -20.0f - (10.0f / 2.0f), 4.0f, 10.0f};
  constexpr Rectangle bottomNozzle = {-70.0f - (4.0f / 2.0f), 20.0f - (10.0f / 2.0f), 4.0f, 10.0f};
  DrawRectangleRounded(topNozzle, 0.2f, 4, colour[2]);
  DrawRectangleRounded(bottomNozzle, 0.2f, 4, colour[2]);
}

void Spaceship::displayHull() const noexcept
{
  // Nose Cone
  constexpr Vector2 nose[] = {{110.0f, 0.0f}, {40.0f, -20.0f}, {50.0f, 20.0f}};
  DrawTriangleFan((Vector2 *)nose, 3, colour[1]);

  // Main Hull Capsule
  DrawEllipse(0, 0, 140.0f / 2.0f, 58.0f / 2.0f, colour[0]);
}

void Spaceship::displayCockpit() const noexcept
{
  rlPushMatrix();
  rlTranslatef(15.0f, -12.0f, 0);
  rlRotatef(8.0f, 0, 0, 1.0f);

  constexpr Color arcFill = {135, 206, 250, 120};
  constexpr float startAngle = 180.0f - (45.0f / 8.0f);
  constexpr float endAngle = 360.0f + (45.0f / 7.0f);
  constexpr float radiusX = 80.0f / 2.0f;
  constexpr float radiusY = 60.0f / 2.0f;

  rlPushMatrix();
  rlTranslatef(3.0f, -13.8f, 0.0f);
  rlScalef(radiusX, radiusY, 1.0f);
  DrawCircleSector({0, 0}, 1.0f, startAngle, endAngle, 36, arcFill);
  rlPopMatrix();

  // Canopy base trim line
  constexpr Rectangle trimBox = {3.0f - (80.0f / 2.0f), -11.0f - (4.0f / 2.0f), 80.0f, 4.0f};
  DrawRectangleRec(trimBox, colour[3]);

  rlPopMatrix(); // Exit Cockpit Matrix
}

// --- High-Level Main Pipeline ---
void Spaceship::display() noexcept
{
  displayTrail();
  displayPilot();

  // Enter Spaceship World Transform Matrix
  rlPushMatrix();
  rlTranslatef(x, y, 0);
  rlScalef(scaleFact, scaleFact, 1.0f);

  displayFins();
  displayLogo();
  displayEngines();
  displayHull();
  displayCockpit();

  rlPopMatrix(); // Exit Spaceship Matrix
}

// Spaceship exhaust trail construction
Spaceship::TrailParticle::TrailParticle(float startX, float startY) noexcept
    : particleX(startX), particleY(startY)
{
  vertexX = randomFloat(-5.0f, -2.0f);
  vertexY = randomFloat(-1.0f, 1.0f);
  pSize = randomFloat(5.0f, 12.0f);
  alpha = 255.0f;
  rotation = 0.0f;
  rotSpeed = randomFloat(-0.06f, 0.06f);

  constexpr unsigned palleteLength = std::size(starBitPalette);
  const unsigned randomIndex = int(randomFloat(0.0f, (float)palleteLength - 1));
  particleColor = starBitPalette[randomIndex];
}

void Spaceship::TrailParticle::update() noexcept
{
  particleX += vertexX;
  particleY += vertexY;
  alpha -= 5.5f;
  rotation += rotSpeed;
}

void Spaceship::TrailParticle::display() noexcept
{
  Color renderColor = particleColor;
  renderColor.a = (unsigned char)std::clamp(alpha, 0.0f, 255.0f);

  // =================================================================
  // CUSTOMISATION VARIABLE
  // Change this number to instantly change the star style!
  // 4 = Classic 4-pointed star (8 segments)
  // 5 = Traditional 5-pointed star (10 segments)
  // 12 = Complex spiked badge/sunburst (24 segments)
  // =================================================================
  constexpr unsigned starPoints = 7;
  constexpr unsigned totalSegments = starPoints * 2;

  // Loop dynamically scales to the total segments needed
  for (unsigned i = 0; i < totalSegments; i++)
  {
    const float angle1 = (((2.0f * PI) / (float)totalSegments) * (float)i) + rotation;
    const float angle2 = (((2.0f * PI) / (float)totalSegments) * (float)(i + 1)) + rotation;

    // The alternating inner/outer radius check automatically scales
    const float radius1 = (i % 2 == 0) ? pSize : pSize * 0.4f;
    const float radius2 = ((i + 1) % 2 == 0) ? pSize : pSize * 0.4f;

    const Vector2 center = {particleX, particleY};
    const Vector2 pointA = {particleX + cosf(angle1) * radius1, particleY + sinf(angle1) * radius1};
    const Vector2 pointB = {particleX + cosf(angle2) * radius2, particleY + sinf(angle2) * radius2};

    // Maintain the working counter-clockwise rendering order
    DrawTriangle(center, pointB, pointA, renderColor);
  }
}

bool Spaceship::TrailParticle::isDead() const noexcept
{
  return alpha <= 0;
}