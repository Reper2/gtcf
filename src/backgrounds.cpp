// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1


#include <raylib.h>
#include <string>
#include <array>
#include "headers/backgrounds.h"
#include "headers/globals.h"

// Define the literal string data array
constexpr std::array<std::string_view, 21> BACKGROUND_IMAGES = {
    "2026052623364700",
    "2026052623392300",
    "2026052700334100",
    "2026052700240100",
    "2026052700220900",
    "2026052623363600",
    "2026052700234400",
    "2026052623450200",
    "2026052700064700",
    "2026052700225200",
    "2026052700224300",
    "2026052700292900",
    "2026052623394600",
    "2026052700314300",
    "2026052623415300",
    "2026052700210100",
    "2026052700153500",
    "2026052700211900",
    "2026052700084100",
    "2026052700542900",
    "2026052700542000"
};

// Generates a valid random index using Raylib's native RNG helper
int GetRandomBackgroundIndex() noexcept {
    // GetRandomValue is inclusive: [min, max]
    return GetRandomValue(0, static_cast<int>(BACKGROUND_IMAGES.size() - 1));
}

// Stitches the full file path string together for Raylib's asset loader
const char* GetBackgroundFilePath(const int index) noexcept {
    if (index < 0 || index >= static_cast<int>(BACKGROUND_IMAGES.size())) {
        return TextFormat("%s%.*s_c.png", IMAGE_URL.c_str(), (int)BACKGROUND_IMAGES[0].length(), BACKGROUND_IMAGES[0].data());
    }
    return TextFormat("%s%.*s_c.png", IMAGE_URL.c_str(), (int)BACKGROUND_IMAGES[index].length(), BACKGROUND_IMAGES[index].data());
}