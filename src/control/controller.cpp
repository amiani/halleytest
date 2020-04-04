#include "controller.h"
#include "chipmunk.hpp"
#include "src/utils.h"

const Action& Controller::getAction() {
  return *actions.back();
}

bool Controller::isObserver() {
  return _isObserver;
}

int Controller::getNextId() {
  return ++lastId;
}

InputController::InputController(
  InputVirtual& device,
  Transform2DComponent& cameraTransform)  : device(device),
                                            cameraTransform(cameraTransform) {
  _isObserver = false;
  id = getNextId();
}

InputController::InputController(
  InputVirtual& device,
  Transform2DComponent& cameraTransform,
  bool isObserver)
  : device(device),
    cameraTransform(cameraTransform) {
  _isObserver = isObserver;
  id = getNextId();
}

const Action& InputController::update(Time t, Observation o, int reward) {
  observations.push_back(std::make_shared<Observation>(o));
  rewards.push_back(reward);
  return update(t);
}

const Action& InputController::update(Time t) {
  device.update(t);
  auto a = std::make_shared<Action>();
  a->throttle = device.isButtonDown(3);
  a->fire = device.isButtonPressed(4);
  a->target = screenToChip(device.getPosition(), halleyToChip(cameraTransform.getGlobalPosition()));
  actions.push_back(a);
  return *a;
}

const Action& RLController::update(Time t, Observation o, int reward) {
  auto a = std::make_shared<Action>(std::move(policy.getAction(o)));
  observations.push_back(std::make_shared<Observation>(std::move(o)));
  rewards.push_back(reward);
  actions.push_back(a);
  return *a;
}

const Action& RLController::update(Time t) {
  throw std::logic_error("RLController must be updated with an Observation");
}