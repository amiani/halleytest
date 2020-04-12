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

const Action& InputController::update(Time t, Observation o, int reward) {
  observations.push_back(std::make_shared<Observation>(o));
  rewards.push_back(reward);
  if (o.terminal && _isObserver) {
    batch.addTrajectory(observations, actions, rewards);
  }
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

auto start = std::chrono::high_resolution_clock::now();
const Action& RLController::update(Time time, Observation o, int reward) {
  rewards.push_back(reward);
  auto a = std::make_shared<Action>(std::move(policy.act(o)));
  if (o.terminal) {
    batch.addTrajectory(observations, actions, rewards);
    observations.clear();
    actions.clear();
    rewards.clear();
    std::cout << "got trajectory: " << batch.getNumTrajectories() << std::endl;
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " milliseconds\n";
    start = std::chrono::high_resolution_clock::now();
    if (batch.getNumTrajectories() > 1000) {
      std::cout << "~~~~GOT BATCH~~~~\n";
      policy = trainer.improve(batch);
      batch = Batch();
      std::cout << "processed batch\n";
    }
  } else {
    observations.push_back(std::make_shared<Observation>(std::move(o)));
    actions.push_back(a);
  }

  return *a;
}

const Action& RLController::update(Time t) {
  throw std::logic_error("RLController must be updated with an Observation");
}