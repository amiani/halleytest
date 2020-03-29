#include "systems/weapon_control_system.h"

class WeaponControlSystem : public WeaponControlSystemBase<WeaponControlSystem> {
public:
  void update(Time t, MainFamily& e) {

  }

  void onMessageReceived(const FireWeaponMessage& msg, MainFamily& e) {
    std::cout << "Fire primary!\n";
  }
};

REGISTER_SYSTEM(WeaponControlSystem)