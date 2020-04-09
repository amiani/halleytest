#pragma once

#include "src/control/mdp.h"
#include "src/control/controller.h"
#include <halley.hpp>
using namespace Halley;

class ControllerService : public Service {
public:
  std::shared_ptr<InputController> makeInputController(InputVirtual& device, Transform2DComponent& cameraTransform);
  std::shared_ptr<RLController> makeRLController();
  Controller& getController(int id);

private:
  std::vector<std::shared_ptr<Controller>> controllers;
};