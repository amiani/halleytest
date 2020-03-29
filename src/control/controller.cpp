#include "controller.h"
#include "chipmunk.hpp"
#include "src/utils.h"

const Action& Controller::getAction() {
  return *actions.back();
}

InputController::InputController(InputVirtual& device) : device(device) {}

const Action& InputController::update(Time t, const cp::Vect& p, Observation& o) {
  observations.push_back(std::make_shared<Observation>(o));
  return update(t, p);
}

const Action& InputController::update(Time t, const cp::Vect& p) {
  device.update(t);
  auto a = std::make_shared<Action>();
  a->throttle = device.isButtonDown(3);
  a->fire = device.isButtonPressed(4);
  a->target = screenToChip(device.getPosition(), p);
  actions.push_back(a);
  return *a;
}