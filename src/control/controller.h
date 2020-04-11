#pragma once

#include <halley.hpp>
#include "mdp.h"
#include "policy.h"
#include "mdp.h"
#include "learner.h"
#include "halley/src/engine/entity/include/halley/entity/components/transform_2d_component.h"

class Controller {
public:
  const Action& getAction();
  virtual const Action& update(Time t, Observation o, int reward) = 0;
  virtual const Action& update(Time t) = 0;
  bool isObserver();

protected:
  int id;
  static int getNextId();
  void saveTrajectory();
  std::vector<std::shared_ptr<Action>> actions;
  std::vector<std::shared_ptr<Observation>> observations;
  std::vector<float> rewards;
  std::vector<Transition> trajectory;
  bool _isObserver = true;

private:
  inline static unsigned int lastId;
};

class InputController : public Controller {
public:
  InputController(InputVirtual& device, Transform2DComponent& cameraTransform);
  InputController(InputVirtual& device, Transform2DComponent& cameraTransform, bool isObserver);
  const Action& update(Time t, Observation o, int reward) override;
  const Action& update(Time t) override;

private:
  InputVirtual device;
  const Transform2DComponent& cameraTransform;
};

class RLController : public Controller {
public:
  const Action& update(Halley::Time t, Observation o, int reward) override;
  const Action& update(Halley::Time t) override;

private:
  Policy policy = Policy("src/control/actor.pt");
  ActorCritic learner = ActorCritic("src/control/actor.pt", "src/control/critic.pt");
};