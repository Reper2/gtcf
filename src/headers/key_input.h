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

#include "penguin.h"
#include "spaceship.h"

bool resolveKeys(const unsigned index, const std::string &keys);
void shootProjectile(const std::string &keys, const std::array<unsigned, 2>index);
void handleMainInp(Spaceship* const mainShip, const std::string &keys);
void handlePilotInp(Penguin* const pilot, const std::string &keys);