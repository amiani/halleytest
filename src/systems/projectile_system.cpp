//
// Created by amiani on 5/2/20.
//

#include "systems/projectile_system.h"
#include "components/team_component.h"

class ProjectileSystem : public ProjectileSystemBase<ProjectileSystem> {
public:
  void update(Halley::Time t) {}

  void onMessageReceived(const ContactMessage& msg, MainFamily& e) {
    if (e.projectile.parent != msg.target) {
      auto phys = e.projectile.physicalDamage;
      auto en = e.projectile.energyDamage;
      getWorld().destroyEntity(e.entityId);
      auto parent = getWorld().tryGetEntity(e.projectile.parent);
      auto target = getWorld().tryGetEntity(msg.target);
      if (parent && target) {
        auto parentTeamComp = parent->tryGetComponent<TeamComponent>();
        auto targetTeamComp = target->tryGetComponent<TeamComponent>();
        if (parentTeamComp && targetTeamComp && parentTeamComp->team != targetTeamComp->team) {
          sendMessage(e.projectile.parent, HitMessage(phys, en));
          sendMessage(msg.target, DamageMessage(phys, en, e.projectile.parent));
        }
      }
    }
  }
};
REGISTER_SYSTEM(ProjectileSystem)
