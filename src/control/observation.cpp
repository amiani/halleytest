#include "observation.h"

std::array<float, 6> EntityData::toBlob() const {
  return {
    position.x,
    position.y,
    rotation,
    velocity.x,
    velocity.y,
    health
  };
}

EntityData EntityData::normalize() const {
  return {
    cp::Vect(position.x/spaceSize.x, position.y/spaceSize.y),
    rotation / PI_CONSTANT,
    cp::Vect(velocity.x/maxVelocity.x, velocity.y/maxVelocity.y),
    health / 500 - 1
  };
}

const cp::Vect EntityData::spaceSize = cp::Vect(1920*4, 1080*4);
const cp::Vect EntityData::maxVelocity = cp::Vect(1000, 1000);

torch::Tensor Observation::toTensor() const {
  std::array<float, dim> blob;
  blob.fill(0);

  auto selfBlob = self.normalize().toBlob();
  std::copy(selfBlob.begin(), selfBlob.end(), blob.begin());
  for (int i = 0; i != 30, i != detectedBodies.size(); ++i) {
    auto entBlob = detectedBodies[i].normalize().toBlob();
    std::copy(entBlob.begin(), entBlob.end(), blob.begin() + (6*(i+1)));
  }
  blob[dim-2] = goal.x/EntityData::spaceSize.x;
  blob[dim-1] = goal.y/EntityData::spaceSize.y;
  return torch::from_blob(blob.data(), {dim}).clone();
}

