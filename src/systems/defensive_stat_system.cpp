#include "systems/defensive_stat_system.h"
using namespace Halley;

class DefensiveStatSystem final : public DefensiveStatSystemBase<DefensiveStatSystem> {
public:
  void update(Time t, MainFamily& e) {}

  void onMessageReceived(const DamageMessage& msg, MainFamily& e) {
    e.health.health -= msg.kinetic;
    if (e.health.health <= 0) {
      getWorld().destroyEntity(e.entityId);
    }
  }

  void onMessageReceived(const ImpactMessage& msg, MainFamily& e) {
    auto damage = msg.kineticEnergy / 20000;
    e.health.health -= damage;
    if (e.observer.hasValue()) {
      e.observer->reward -= damage;
    }
  }
};

REGISTER_SYSTEM(DefensiveStatSystem)