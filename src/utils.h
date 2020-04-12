#pragma once

#include "chipmunk.hpp"
#include <halley.hpp>
#include "torch/torch.h"

Halley::Vector2f chipToScreen(cp::Vect, cp::Vect);
cp::Vect screenToChip(Halley::Vector2f, cp::Vect);
Halley::Vector2f chipToHalley(const cp::Vect&);
cp::Vect halleyToChip(const Halley::Vector2f);

const torch::Device DEVICE(torch::kCPU);

enum BodyType {
  PLAYERSHIPBODY,
  PROJECTILEBODY,
  ENEMYSHIPBODY,
  DETECTORBODY,
  ASTEROIDBODY,
  GOALBODY
};

enum CollisionFilter {
  PLAYERHULL = (1u << 0),
  PLAYERDETECTOR = (1u << 1),
  PLAYERPROJECTILE = (1u << 2),
  GOAL = (1u << 3),
  ASTEROID = (1u << 4),
  BOUNDARY = (1u << 5)
};