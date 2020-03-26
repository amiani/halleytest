#include "utils.h"

const auto screenSize = Halley::Vector2i(1920, 1080) / 5 * 4;

Halley::Vector2f chipToScreen(cp::Vect p, cp::Vect camPos) {
  const auto relPos = p - camPos;
  return Halley::Vector2f(relPos.x, screenSize.y - relPos.y);
}

cp::Vect screenToChip(Halley::Vector2f p, cp::Vect camPos) {
  const auto invPos = cp::Vect(p.x - screenSize.x / 2, screenSize.y / 2 - p.y);
  return invPos + camPos;
}