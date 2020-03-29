#include "controller_service.h"
#include "src/control/controller.h"

std::shared_ptr<InputController> ControllerService::makeInputController(InputVirtual& device) {
  auto c = std::make_shared<InputController>(device);
  controllers.push_back(c);
  return c;
}

RLController& ControllerService::makeRLController() {
  //actors.push_back(RLController());
}

Controller& ControllerService::getController(int id) {
  return *controllers[id];
}