#pragma once

#include <halley.hpp>
#include "mdp.h"
#include "halley/src/engine/entity/include/halley/entity/components/transform_2d_component.h"
#include "trainer.h"

class Controller {
public:
  virtual const Action& update(Time t, Observation o, float reward) = 0;
  virtual const Action& update(Time t) = 0;
  bool isObserver();

protected:
  int id;
  static int getNextId();
  std::vector<std::shared_ptr<Observation>> observations;
  std::vector<std::shared_ptr<Action>> actions;
  std::vector<float> rewards;
  TrajBatch batch;
  bool _isObserver = true;

private:
  inline static unsigned int lastId;
};

class InputController : public Controller {
public:
  InputController(InputVirtual& device, Transform2DComponent& cameraTransform);
  InputController(InputVirtual& device, Transform2DComponent& cameraTransform, bool isObserver);
  const Action& update(Time t, Observation o, float reward) override;
  const Action& update(Time t) override;

private:
  InputVirtual device;
  const Transform2DComponent& cameraTransform;
};

class RLController : public Controller {
public:
  RLController(std::unique_ptr<Trainer>, bool train = true);
  RLController(std::shared_ptr<Actor>, bool train = true);
  const Action& update(Halley::Time t, Observation o, float r) override;
  const Action& update(Halley::Time t) override;

private:
  bool train;
  std::unique_ptr<Trainer> trainer;
  //std::shared_ptr<Actor> actor;
  std::shared_ptr<Actor> actor;
};