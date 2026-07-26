// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1


#pragma once
#include <string>
#include <array>

// Define the asset location path
const std::string IMAGE_URL = "resources/images/";

// Declare the list of background asset names globally
extern const std::array<std::string_view, 21> BACKGROUND_IMAGES;

// Helper Functions
int GetRandomBackgroundIndex() noexcept;
const char* GetBackgroundFilePath(const int index) noexcept;