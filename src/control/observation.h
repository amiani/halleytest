#pragma once

#include "chipmunk.hpp"
#include <halley.hpp>
using namespace Halley;

struct EntityData {
  cp::Vect position;
  double rotation;
  cp::Vect velocity;
  int health;
  std::array<float, 6> toBlob();
};

struct Observation {
  EntityData self;
  //EntityData enemies[1];
  std::vector<EntityData> detectedBodies;
  //EntityData allies[1];
};