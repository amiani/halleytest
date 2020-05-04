#pragma once

#include "chipmunk.hpp"
#include "torch/torch.h"
#include <halley.hpp>
using namespace Halley;

enum Allegiance {
  NEUTRAL,
  ALLY,
  ENEMY
};

struct StateBase {
  cp::Vect position;
  float rotation;
  cp::Vect velocity;
  float health;

  inline static const int dim = 6;
  template<size_t size>
  std::array<float, size> toArray() const;

protected:
  StateBase(const cp::Body& body, int health);
};

struct EntityState final : public StateBase {
  EntityState(const cp::Body& body, int health, Allegiance allegiance);
  Allegiance allegiance;
  inline static const int dim = StateBase::dim + 3;
  std::array<float, EntityState::dim> toArray() const;
};

struct SelfState final : public StateBase {
  SelfState(const cp::Body& body, int health);
  float angularVelocity;
  inline static const int dim = StateBase::dim + 1;
  std::array<float, SelfState::dim> toArray() const;
};

struct Observation {
  Observation(SelfState s) : self(s) {}
  UUID uuid;
  bool terminal;
  SelfState self;
  std::vector<EntityState> enemies;
  std::vector<EntityState> detectedBodies;
  //EntityData allies[1];
  torch::Tensor toTensor() const;
  inline static const int numEnemies = 2;
  inline static const int numDetected = 2;
  inline static const int dim = SelfState::dim + EntityState::dim * numEnemies + EntityState::dim * numDetected;
};

