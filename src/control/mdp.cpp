#include "mdp.h"

std::array<float, 6> EntityData::toBlob() {
  return {
    position.x,
    position.y,
    rotation,
    velocity.x,
    velocity.y,
    health
  };
}

std::array<float, 6*31> Observation::toBlob() {
  std::array<float, 6*31> blob;
  blob.fill(0);
  
  auto selfBlob = self.toBlob();
  std::copy(selfBlob.begin(), selfBlob.end(), blob.begin());
  for (int i = 0; i != 30, i != detectedBodies.size(); ++i) {
    auto entBlob = detectedBodies[i].toBlob();
    std::copy(entBlob.begin(), entBlob.end(), blob.begin() + (6*(i+1)));
  }
  return blob;
}