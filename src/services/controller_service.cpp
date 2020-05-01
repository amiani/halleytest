#include <src/control/dqn/dqn_trainer.h>
#include <src/control/sac/sac_trainer.h>
#include <src/control/sac/sac_actor.h>
#include "controller_service.h"
#include "src/control/controller.h"

ControllerService::ControllerService(String actorPath, String critic1Path, String critic2Path)
  : actorPath(actorPath), critic1Path(critic1Path), critic2Path(critic2Path), actor(std::make_shared<SACActor>()){}

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
