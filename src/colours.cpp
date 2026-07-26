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
#include "headers/colours.h"

constexpr Color CYAN = {0, 255, 255, 255};
constexpr Color TEAL = {83, 195, 189, 255};
constexpr Color GREY = {140, 142, 145, 255};
constexpr Color GREY2 = {230, 230, 230, 255};
constexpr Color redRGBA = {228, 34, 23, 255};
constexpr Color greenRGBA = {79, 170, 75, 255};
constexpr Color blueRGBA = {0, 0, 139, 255};
constexpr std::array<Color, 5> fireballColours = {{
    Color{240, 40, 10, 180},  // 0: Outer Plasma Flares
    Color{255, 65, 0, 255},   // 1: Main fireball outer body
    Color{255, 130, 0, 255},  // 2: Dynamic flame crown accent
    Color{255, 200, 10, 255}, // 3: Inner core
    Color{255, 255, 210, 255} // 4: Fireball core spark white-yellow flash!
}};
constexpr std::array<Color, 5> iceballColours = {{
    Color{90, 40, 230, 160},   // 0: Outer icy vapour layer
    Color{0, 160, 255, 255},   // 1: Main outer layer
    Color{100, 225, 255, 255}, // 2: Inner layer of icy shards
    Color{190, 245, 255, 255}, // 3: Cryogenic core
    Color{255, 255, 255, 255}  // 4: Core absolute zero flash
}};
constexpr std::array<std::array<Color, 2>, 3> alienColours = {{
    {Color{160, 80, 255, 255}, Color{0, 255, 150, 255}},  // Purple Bug Alien
    {Color{255, 40, 40, 255}, Color{255, 255, 255, 255}}, // Angry Red Arrowhead Alien
    {Color{230, 190, 20, 255}, Color{30, 30, 30, 255}}    // Yellow Armoured Alien
}};
constexpr std::array<Color, 4> spaceshipColours = {{
    Color{235, 235, 240, 255}, // Hull
    Color{220, 50, 50, 255},   // Fins
    Color{0, 255, 230, 255},   // Engine Glow
    Color{70, 75, 90, 255}     // Cockpit Trim
}};
constexpr std::array<Color, 4> grandTourStyle = {{
    Color{235, 230, 215, 255}, // Hull: Creamy Vintage White
    Color{40, 55, 70, 255},    // Fins/Nose: Deep Midnight Slate
    Color{230, 90, 40, 255},   // Engine Glow: Vivid Burnt Orange
    Color{190, 140, 50, 255}   // Trim Line: Retro Ochre Yellow
}};
constexpr std::array<Color, 4> enceledusStyle = {{
    Color{25, 75, 85, 255},    // Hull: Deep Sea Petrol Teal
    Color{150, 190, 180, 255}, // Fins/Nose: Muted Sage/Mint Grey
    Color{255, 40, 130, 255},  // Engine Glow: Neon Hot Magenta
    Color{240, 220, 140, 255}  // Trim Line: Pale Pastel Gold
}};
constexpr std::array<Color, 4> marsPioneerStyle = {{
    Color{165, 55, 45, 255},  // Hull: Terracotta Rust Red
    Color{220, 160, 45, 255}, // Fins/Nose: Harvest Mustard Yellow
    Color{0, 220, 210, 255},  // Engine Glow: Bright Cyan Flare
    Color{50, 50, 55, 255}    // Trim Line: Dark Industrial Charcoal
}};
constexpr std::array<Color, 6> starBitPalette = {{
    Color{255, 225, 40, 255}, // Yellow
    Color{255, 75, 110, 255}, // Coral / Pinkish Red
    Color{45, 180, 255, 255}, // Cyan / Sky Blue
    Color{75, 230, 80, 255},  // Emerald Green
    Color{180, 75, 255, 255}, // Purple
    Color{235, 245, 255, 255} // Silver / White
}};
constexpr Color iceFill = {110, 210, 255, 140};     // Semi-transparent icy blue
constexpr Color iceStroke = {220, 245, 255, 220};   // Bright ice border
constexpr Color glareColour = {255, 255, 255, 100}; // White glare streak

const std::vector<std::pair<std::string, Color>> colourMap = {
    {"blue", blueRGBA},
    {"red", redRGBA},
    {"green", greenRGBA},
    {"cyan", CYAN},
    {"teal", TEAL},
    {"coral", starBitPalette[1]},
    {"skyblue", starBitPalette[2]},
    {"emerald", starBitPalette[3]},
    {"purple", starBitPalette[4]},
    {"silver", starBitPalette[5]}};
const std::vector<std::pair<std::string, std::array<Color, 4>>> shipColourMap = {
    {"grandTour", grandTourStyle},
    {"enceledus", enceledusStyle},
    {"marsPioneer", marsPioneerStyle}};