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
  bool terminal;
  EntityData self;
  //EntityData enemies[1];
  std::vector<EntityData> detectedBodies;
  //EntityData allies[1];
  std::array<float, 6*31> toBlob();
};

struct Action {
  bool throttle = false;
  bool fire = false;
  cp::Vect target = cp::Vect();
};

struct Transition {
  Observation observation;
  Action action;
  float reward;
  Observation next;
};

typedef std::vector<Transition> Trajectory;