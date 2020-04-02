#pragma once

#include "chipmunk.hpp"
#include <halley.hpp>
using namespace Halley;

struct EntityData {
  cp::Vect position;
  double rotation;
  cp::Vect velocity;
  int health;
};

struct Observation {
  EntityData self;
  EntityData enemies[1];
  EntityData envBodies[30];
  //EntityData allies[1];
};