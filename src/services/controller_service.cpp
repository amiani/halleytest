#include <src/control/dqn/dqn_trainer.h>
#include <src/control/sac/sac_trainer.h>
#include <src/control/sac/sac_actor.h>
#include "controller_service.h"
#include "src/control/controller.h"

ControllerService::ControllerService(bool loadFromDisk, bool train)
  : replayBuffer(std::make_shared<ReplayBuffer>(loadFromDisk)), actor(std::make_shared<SACActor>(*replayBuffer, loadFromDisk, train)) {}

std::shared_ptr<InputController> ControllerService::makeInputController(
  InputVirtual& device,
  Transform2DComponent& cameraTransform) {
  auto c = std::make_shared<InputController>(device, cameraTransform);
  controllers.push_back(c);
  return c;
}

std::shared_ptr<RLController> ControllerService::makeRLController() {
  auto c = std::make_shared<RLController>(actor);
  rlControllers.push_back(c);
  return c;
}
