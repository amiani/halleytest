#pragma once

#include "chipmunk.hpp"
#include "torch/torch.h"
#include <halley.hpp>
using namespace Halley;

struct EntityData {
  cp::Vect position;
  float rotation;
  cp::Vect velocity;
  float health;
  std::array<float, 6> toBlob() const;
  EntityData normalize() const;
  static const cp::Vect spaceSize;
  static const cp::Vect maxVelocity;
  static const int maxHealth = 1000;
};

struct Observation {
  bool terminal;
  cp::Vect goal;
  float angularVelocity;
  EntityData self;
  //EntityData enemies[1];
  std::vector<EntityData> detectedBodies;
  //EntityData allies[1];
  torch::Tensor toTensor() const;
  inline static const int dim = 6*31 + 3;
};

