#include "controller.h"
#include "chipmunk.hpp"
#include "src/utils.h"
#include <chrono>

Controller::Controller() : uuid(Halley::UUID::generate()) {}

bool Controller::isObserver() {
  return _isObserver;
}

int Controller::getNextId() {
  return ++lastId;
}

Halley::UUID Controller::getUUID() const {
  return uuid;
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
  bool isObserver) :  device(device),
                      cameraTransform(cameraTransform) {
  _isObserver = isObserver;
  id = getNextId();
}

Action InputController::update(Time t, Observation o, float reward) {
  //add tuple to internal batch?
  return update(t);
}

Action InputController::update(Time t) {
  device.update(t);
  Action a;
  a.throttle = device.isButtonDown(3);
  a.fire = device.isButtonPressed(4);
  if (device.isButtonDown(2)) {
    a.direction = LEFT;
  } else if (device.isButtonDown(1)) {
    a.direction = RIGHT;
  }
  return a;
}

RLController::RLController(std::shared_ptr<Actor> actor) : actor(actor) {}

auto start = std::chrono::high_resolution_clock::now();
Action RLController::update(Time time, Observation o, float r) {
  return actor->act(o, r);
}

Action RLController::update(Time t) {
  throw std::logic_error("RLController must be updated with an Observation");
}