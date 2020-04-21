#include "controller.h"
#include "chipmunk.hpp"
#include "src/utils.h"
#include <chrono>
#include <src/control/sac/sac_trainer.h>

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

RLController::RLController(std::unique_ptr<Trainer> t)
  : trainer(std::move(t)), actor(trainer->getActor()), train(true) {}

RLController::RLController(std::shared_ptr<Actor> actor) : actor(actor), train(false) {}

auto start = std::chrono::high_resolution_clock::now();
const Action& RLController::update(Time time, Observation o, float r) {
  rewards.push_back(r);
  auto a = std::make_shared<Action>(actor->act(o));
  if (train) {
    trainer->addStep(o, *a, r);
  }
  if (o.terminal) {
    float total = 0;
    for (auto r : rewards) total += r;
    std::cout << total << std::endl;
    observations.clear();
    actions.clear();
    rewards.clear();
  }
    /*
    std::cout << "trajectory: " << batch.getNumTrajectories() << std::endl;
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " milliseconds\n";
    start = std::chrono::high_resolution_clock::now();
    if (batch.getNumTrajectories() > 40) {
      std::cout << "\n~~~~GOT BATCH~~~~\n";
      trainer.improve();
      batch = TrajBatch();
    }
  } else {
    observations.push_back(std::make_shared<Observation>(std::move(o)));
    actions.push_back(a);
  }
  */

  return *a;
}

const Action& RLController::update(Time t) {
  throw std::logic_error("RLController must be updated with an Observation");
}