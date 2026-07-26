// Copyright (c) 2026 Ethan Graham. All Rights Reserved.
// Project Repository: https://github.com/Reper2/gtcf
// Distributed under custom Source-Available terms. Architectural reuse permitted.
// See root LICENSE.txt or repository for terms, restrictions, and abandonment exceptions.

// This project is inspired by the Grand Tour poster by NASA,
// using concepts from the poster including spaceships and exhaust trails.
// https://www.jpl.nasa.gov/galleries/visions-of-the-future/#grid-127451-1


#pragma once
#include "raylib.h"
#include <vector>
#include <string>

class Spaceship;

void changeGameState(std::string newState);
void resetGame();
void spawnSpacePenguins();
void setup();