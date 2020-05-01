#pragma once

#include <halley.hpp>
#include "observation.h"
#include "halley/src/engine/entity/include/halley/entity/components/transform_2d_component.h"
#include "trainer.h"
#include "action.h"

class Controller {
public:
  Controller();
  virtual Action update(Time t, Observation o, float reward) = 0;
  virtual Action update(Time t) = 0;
  bool isObserver();
  Halley::UUID getUUID() const;

protected:
  int id;
  static int getNextId();
  std::vector<std::shared_ptr<Observation>> observations;
  std::vector<std::shared_ptr<Action>> actions;
  std::vector<float> rewards;
  bool _isObserver = true;

private:
  inline static unsigned int lastId;
  Halley::UUID uuid;
};

class InputController : public Controller {
public:
  InputController(InputVirtual& device, Transform2DComponent& cameraTransform);
  InputController(InputVirtual& device, Transform2DComponent& cameraTransform, bool isObserver);
  Action update(Time t, Observation o, float reward) override;
  Action update(Time t) override;

private:
  InputVirtual device;
  const Transform2DComponent& cameraTransform;
};

class RLController : public Controller {
public:
  RLController(std::shared_ptr<Actor> actor);
  Action update(Halley::Time t, Observation o, float r) override;
  Action update(Halley::Time t) override;

private:
  std::shared_ptr<Actor> actor;
};