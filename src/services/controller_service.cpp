#include <src/control/dqn/dqn_trainer.h>
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
  auto trainer = std::make_unique<SACTrainer>();
  auto actor = trainer->getActor();
  auto c = std::make_shared<RLController>(std::move(trainer), actor, true);
  rlControllers.push_back(c);
  return c;
}

Controller& ControllerService::getController(int id) {
  return *controllers[id];
}

std::shared_ptr<RLController> ControllerService::getRLController() {
  return rlControllers.empty() ? makeRLController() : rlControllers[0];
}
