#pragma once

#include "src/control/observation.h"
#include "src/control/controller.h"
#include <halley.hpp>
using namespace Halley;

class ControllerService : public Service {
public:
  ControllerService(String, String, String);
  std::shared_ptr<InputController> makeInputController(InputVirtual& device, Transform2DComponent& cameraTransform);
  std::shared_ptr<RLController> makeRLController();

private:
  String actorPath, critic1Path, critic2Path;
  std::shared_ptr<Actor> actor;
  std::vector<std::shared_ptr<Controller>> controllers;
  std::vector<std::shared_ptr<RLController>> rlControllers;
};