#pragma once

#include "chipmunk.hpp"
#include "torch/torch.h"
#include <halley.hpp>
using namespace Halley;

struct EntityData {
  cp::Vect position;
  double rotation;
  cp::Vect velocity;
  float health;
  std::array<float, 6> toBlob();
  EntityData normalize();
  static const cp::Vect spaceSize;
  static const cp::Vect maxVelocity;
  static const int maxHealth = 1000;
};

struct Observation {
  bool terminal;
  EntityData self;
  //EntityData enemies[1];
  std::vector<EntityData> detectedBodies;
  //EntityData allies[1];
  torch::Tensor toTensor();
  static const int dim = 6*31;
};

struct Action {
  bool throttle = false;
  bool fire = false;
  cp::Vect target = cp::Vect();
  torch::Tensor logProb;
};

struct Transition {
  Observation observation;
  Action action;
  float reward;
  Observation next;
};

typedef std::vector<Transition> Trajectory;