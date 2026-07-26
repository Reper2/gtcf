// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1


#pragma once
#include <raylib.h>
#include <array>

extern const Color CYAN;
extern const Color TEAL;
extern const Color GREY;
extern const Color GREY2;
extern const Color redRGBA;
extern const Color greenRGBA;
extern const Color blueRGBA;
extern const std::array<Color, 5> fireballColours;
extern const std::array<Color, 5> iceballColours;
extern const std::array<std::array<Color, 2>, 3> alienColours;
extern const std::array<Color, 4> spaceshipColours;
extern const std::array<Color, 4> grandTourStyle;
extern const std::array<Color, 4> enceledusStyle;
extern const std::array<Color, 4> marsPioneerStyle;
extern const std::array<Color, 6> starBitPalette;
extern const Color iceFill;
extern const Color iceStroke;
extern const Color glareColour;

extern const std::vector<std::pair<std::string, Color>> colourMap;
extern const std::vector<std::pair<std::string, std::array<Color, 4>>> shipColourMap;