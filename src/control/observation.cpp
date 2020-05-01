#include "observation.h"

StateBase::StateBase(const cp::Body& body, int health)
  : position(body.getPosition()),
  velocity(body.getVelocity()),
  health(health) {
  float bodyAngle = static_cast<float>(body.getAngle());
  if (bodyAngle > 0)
    rotation = (fmod(bodyAngle + M_PI, 2 * M_PI) - M_PI) / M_PI;
  else
    rotation = (fmod(bodyAngle - M_PI, 2 * M_PI) + M_PI) / M_PI;
}

template<size_t size>
std::array<float, size> StateBase::toArray() const {
  return {
    position.x,
    position.y,
    rotation,
    velocity.x,
    velocity.y,
    health
  };
}

EntityState::EntityState(const cp::Body& body, int health, Allegiance allegiance)
: StateBase(body, health), allegiance(allegiance) {}

std::array<float, EntityState::dim> EntityState::toArray() const {
  auto base = StateBase::toArray<dim>();
  std::copy(base.begin(), base.end(), base.begin());
  base[dim-3] = allegiance == ALLY;
  base[dim-2] = allegiance == ENEMY;
  base[dim-1] = 1;  //this is to distinguish between an entity with data close to zero and an empty entity
  return base;
}

SelfState::SelfState(const cp::Body& body, int health)
: StateBase(body, health), angularVelocity(body.getAngularVelocity()) {}

std::array<float, SelfState::dim> SelfState::toArray() const {
  auto base = StateBase::toArray<dim>();
  base.back() = angularVelocity;
  return base;
}

torch::Tensor Observation::toTensor() const {
  std::array<float, dim> blob{};
  auto iter = blob.begin();

  auto selfData = self.toArray();
  iter = std::copy(selfData.begin(), selfData.end(), iter);

  for (auto& enemy : enemies) {
    auto enemyData = enemy.toArray();
    iter = std::copy(enemyData.begin(), enemyData.end(), iter);
  }

  auto size = detectedBodies.size();
  for (int i = 0; i != numDetected && i != size; ++i) {
    auto entData = detectedBodies[i].toArray();
    iter = std::copy(entData.begin(), entData.end(), iter);
  }
 return torch::from_blob(blob.data(), {dim}).clone();
}

