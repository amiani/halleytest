#include <src/control/sac/sac_trainer.h>
#include "controller_service.h"
#include "src/control/controller.h"

ControllerService::ControllerService(String actorPath, String critic1Path, String critic2Path)
  : actorPath(actorPath), critic1Path(critic1Path), critic2Path(critic2Path) {}

std::shared_ptr<InputController> ControllerService::makeInputController(
  InputVirtual& device,
  Transform2DComponent& cameraTransform) {
  auto c = std::make_shared<InputController>(device, cameraTransform);
  controllers.push_back(c);
  return c;
}

std::shared_ptr<RLController> ControllerService::makeRLController() {
  auto trainer = std::make_unique<SACTrainer>(actorPath, critic1Path, critic2Path);
  auto c = std::make_shared<RLController>(std::move(trainer));
  rlControllers.push_back(c);
  return c;
}

Controller& ControllerService::getController(int id) {
  return *controllers[id];
}

std::shared_ptr<RLController> ControllerService::getRLController() {
  return rlControllers.empty() ? makeRLController() : rlControllers[0];
}
