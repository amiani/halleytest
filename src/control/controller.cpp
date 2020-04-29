#include "controller.h"
#include "chipmunk.hpp"
#include "src/utils.h"
#include <chrono>

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
  bool isObserver) :  device(device),
                      cameraTransform(cameraTransform) {
  _isObserver = isObserver;
  id = getNextId();
}

const Action& InputController::update(Time t, Observation o, float reward) {
  //add tuple to internal batch?
  return update(t);
}

const Action& InputController::update(Time t) {
  device.update(t);
  auto a = std::make_shared<Action>();
  a->throttle = device.isButtonDown(3);
  a->fire = device.isButtonPressed(4);
  if (device.isButtonDown(2)) {
    a->direction = LEFT;
  } else if (device.isButtonDown(1)) {
    a->direction = RIGHT;
  }
  actions.push_back(a);
  return *a;
}

RLController::RLController(std::unique_ptr<Trainer> t, bool train)
  : trainer(std::move(t)), actor(trainer->getActor()), train(train) {}

RLController::RLController(std::unique_ptr<Trainer> t, std::shared_ptr<Actor> actor, bool train)
  : trainer(std::move(t)), actor(actor), train(train) {}

RLController::RLController(std::shared_ptr<Actor> actor, bool train) : actor(actor), train(train) {}

auto start = std::chrono::high_resolution_clock::now();
const Action& RLController::update(Time time, Observation o, float r) {
  auto a = std::make_shared<Action>(actor->act(o));
  if (train) {
    trainer->addStep(o, *a, r);
  }
  return *a;
}

const Action& RLController::update(Time t) {
  throw std::logic_error("RLController must be updated with an Observation");
}