#pragma once

#include "src/control/observation.h"
#include "src/control/controller.h"
#include "src/control/replay_buffer.h"
#include <halley.hpp>
using namespace Halley;

class ControllerService : public Service {
public:
  ControllerService(bool loadFromDisk, bool train);
  std::shared_ptr<InputController> makeInputController(InputVirtual& device, Transform2DComponent& cameraTransform);
  std::shared_ptr<RLController> makeRLController();

private:
  std::shared_ptr<ReplayBuffer> replayBuffer;
  std::shared_ptr<Actor> actor;
  std::vector<std::shared_ptr<Controller>> controllers;
  std::vector<std::shared_ptr<RLController>> rlControllers;
};