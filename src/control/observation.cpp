#include "observation.h"

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