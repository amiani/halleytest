#pragma once

#include "chipmunk.hpp"
#include <halley.hpp>

Halley::Vector2f chipToScreen(cp::Vect, cp::Vect);
cp::Vect screenToChip(Halley::Vector2f, cp::Vect);
Halley::Vector2f chipToHalley(const cp::Vect&);
cp::Vect halleyToChip(const Halley::Vector2f);

enum BodyType {
  PLAYERSHIP,
  PROJECTILE,
  ENEMYSHIP,
  SENSOR,
  ASTEROID
};