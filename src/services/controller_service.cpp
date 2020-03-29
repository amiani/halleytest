#include "controller_service.h"
#include "src/control/controller.h"

std::shared_ptr<InputController> ControllerService::makeInputController(InputVirtual& device) {
  controllers.push_back(std::make_unique<InputController>(device));
}

RLController& ControllerService::makeRLController() {
  //actors.push_back(RLController());
}

Controller& ControllerService::getController(int id) {
  return *controllers[id];
}