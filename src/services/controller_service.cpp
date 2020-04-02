#include "controller_service.h"
#include "src/control/controller.h"

std::shared_ptr<InputController> ControllerService::makeInputController(
  InputVirtual& device,
  Transform2DComponent& cameraTransform) {
  auto c = std::make_shared<InputController>(device, cameraTransform);
  controllers.push_back(c);
  return c;
}

std::shared_ptr<RLController> ControllerService::makeRLController() {
  auto c = std::make_shared<RLController>();
  controllers.push_back(c);
  return c;
}

Controller& ControllerService::getController(int id) {
  return *controllers[id];
}