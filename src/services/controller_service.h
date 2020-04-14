#pragma once

#include "src/control/mdp.h"
#include "src/control/controller.h"
#include "src/control/trainer.h"
#include <halley.hpp>
using namespace Halley;

class ControllerService : public Service {
public:
  ControllerService(String, String);
  std::shared_ptr<InputController> makeInputController(InputVirtual& device, Transform2DComponent& cameraTransform);
  std::shared_ptr<RLController> makeRLController();
  std::shared_ptr<RLController> getRLController();
  Controller& getController(int id);

private:
  std::vector<std::shared_ptr<Controller>> controllers;
  std::vector<std::shared_ptr<RLController>> rlControllers;
  String actorPath, criticPath;
};