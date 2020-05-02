//
// Created by amiani on 5/2/20.
//

#include "systems/projectile_system.h"

class ProjectileSystem : public ProjectileSystemBase<ProjectileSystem> {
public:
  void update(Halley::Time t) {}

  void onMessageReceived(const ContactMessage& msg, MainFamily& e) {
    auto ki = e.projectile.kineticDamage;
    auto en = e.projectile.energyDamage;
    getWorld().destroyEntity(e.entityId);
    sendMessage(e.projectile.parent, HitMessage(ki, en));
    sendMessage(msg.target, DamageMessage(ki, en));
  }
};
REGISTER_SYSTEM(ProjectileSystem)
