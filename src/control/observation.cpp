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
  float rotationScaled;
  if (rotation > 0)
    rotationScaled = (fmod(rotation + M_PI, 2 * M_PI) - M_PI) / M_PI;
  else
    rotationScaled = (fmod(rotation - M_PI, 2 * M_PI) + M_PI) / M_PI;

  return {
    cp::Vect(position.x/spaceSize.x, position.y/spaceSize.y),
    rotationScaled,
    cp::Vect(velocity.x/maxVelocity.x, velocity.y/maxVelocity.y),
    health / 500 - 1
  };
}

const cp::Vect EntityData::spaceSize = cp::Vect(1920, 1080);
const cp::Vect EntityData::maxVelocity = cp::Vect(400, 400);

torch::Tensor Observation::toTensor() const {
  std::array<float, dim> blob;
  blob.fill(0);

  std::cout << "detectedBodies: " << detectedBodies.size() << std::endl;
  auto selfBlob = self.normalize().toBlob();
  std::copy(selfBlob.begin(), selfBlob.end(), blob.begin());
  for (int i = 0; i != 30, i != detectedBodies.size(); ++i) {
    auto entBlob = detectedBodies[i].normalize().toBlob();
    std::copy(entBlob.begin(), entBlob.end(), blob.begin() + (6*(i+1)));
  }
  blob[dim-3] = angularVelocity/20;
  blob[dim-2] = goal.x/1920;
  blob[dim-1] = goal.y/1080;
  return torch::from_blob(blob.data(), {dim}).clone();
}

