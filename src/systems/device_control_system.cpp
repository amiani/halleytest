#include <systems/device_control_system.h>

using namespace Halley;

class DeviceControlSystem : public DeviceControlSystemBase<DeviceControlSystem> {
public:
  void update(Time time, MainFamily& e) {
    //Action action;
    if (e.deviceControl.device->isButtonPressed(3)) {
      e.body.body->applyForceAtLocalPoint(cp::Vect(0, 1), cp::Vect(0, 0));
    }
    const cp::Vect bodyPos = e.body.body->getPosition();
    const auto windowSize = Vector2f(1920, 1080) / 5 * 4;
    const auto mousePos = e.deviceControl.device->getPosition();
    const auto mouseWorldPos = mousePos - windowSize / 2;
    
    //e.body.body->setAngle(angle);
  }
};

REGISTER_SYSTEM(DeviceControlSystem)