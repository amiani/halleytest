#include "controller_service.h"
#include "src/control/controller.h"

ControllerService::ControllerService(String actorPath, String criticPath) : actorPath(actorPath), criticPath(criticPath) {}

std::shared_ptr<InputController> ControllerService::makeInputController(
  InputVirtual& device,
  Transform2DComponent& cameraTransform) {
  auto c = std::make_shared<InputController>(device, cameraTransform);
  controllers.push_back(c);
  return c;
}

std::shared_ptr<RLController> ControllerService::makeRLController() {
  auto c = std::make_shared<RLController>(actorPath, criticPath);
  rlControllers.push_back(c);
  return c;
}

Controller& ControllerService::getController(int id) {
  return *controllers[id];
}

std::shared_ptr<RLController> ControllerService::getRLController() {
  return rlControllers.empty() ? makeRLController() : rlControllers[0];
}
