#include "systems/control_system.h"
#include "chipmunk.hpp"

class ControlSystem final : public ControlSystemBase<ControlSystem> {
public:
  void update(Halley::Time time, MainFamily& e) {
    auto body = e.body.body;
    auto a = e.shipControl.controller->update(time, body->getPosition());

    if (a.throttle) {
      body->applyForceAtLocalPoint(cp::Vect(500, 0), cp::Vect(0, 0));
    }

    cp::Vect bodyPos = body->getPosition();
    double angle = cp::Vect::toAngle(a.target - bodyPos);
    body->setAngle(angle);
  }
};

REGISTER_SYSTEM(ControlSystem)