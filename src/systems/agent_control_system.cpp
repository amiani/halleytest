#include <systems/rl_control_system.h>

using namespace Halley;

class RLControlSystem : public RLControlSystemBase<RLControlSystem> {
public:
  void update(Time time, MainFamily& e) {

  }
};

REGISTER_SYSTEM(RLControlSystem)