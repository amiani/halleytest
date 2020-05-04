#include "utils.h"

const auto screenSize = Halley::Vector2i(1920, 1080) / 5 * 4;

Halley::Vector2f chipToScreen(cp::Vect p, cp::Vect camPos) {
  const auto relPos = p - camPos;
  return Halley::Vector2f(relPos.x + screenSize.x / 2, screenSize.y / 2 + relPos.y);
}

cp::Vect screenToChip(Halley::Vector2f p, cp::Vect camPos) {
  const auto invPos = cp::Vect(p.x - screenSize.x / 2, p.y - screenSize.y / 2);
  return invPos + camPos;
}

Halley::Vector2f chipToHalley(const cp::Vect& v) {
  return Halley::Vector2f(v.x, v.y);
}

cp::Vect halleyToChip(Halley::Vector2f v) {
  return cp::Vect(v.x, v.y);
}

bool isAnyNAN(const torch::Tensor& t) {
  return torch::isnan(t).any().item<bool>();
}

bool isAnyZero(const torch::Tensor& t) {
  return t.eq(0).any().item<bool>();
}
