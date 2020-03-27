#include <systems/device_control_system.h>
#include "src/utils.h"

using namespace Halley;

class DeviceControlSystem : public DeviceControlSystemBase<DeviceControlSystem> {
public:
  void update(Time time, MainFamily& e) {
    //Action action;
    const auto body = e.body.body;
    if (e.deviceControl.device->isButtonDown(3)) {
      body->applyForceAtLocalPoint(cp::Vect(500, 0), cp::Vect(0, 0));
    }
    const cp::Vect bodyPos = body->getPosition();
    const auto mousePos = e.deviceControl.device->getPosition();
    const auto mouseWorldPos = screenToChip(mousePos, bodyPos);
    //std::cout << mouseWorldPos << std::endl;
    const double angle = cp::Vect::toAngle(mouseWorldPos - bodyPos);
    body->setAngle(angle);
  }
};

REGISTER_SYSTEM(DeviceControlSystem)