#include <gen/cpp/components/projectile_component.h>
#include "systems/weapon_control_system.h"
#include "components/body_component.h"
#include "components/shape_component.h"
#include "components/sprite_component.h"
#include "components/lifetime_component.h"
#include "halley/src/engine/entity/include/halley/entity/components/transform_2d_component.h"
#include "chipmunk.hpp"
#include "src/utils.h"

class WeaponControlSystem : public WeaponControlSystemBase<WeaponControlSystem> {
public:
  void update(Time t, MainFamily& e) {
    if (e.cooldown.timeLeft > 0)  e.cooldown.timeLeft -= t;
  }

  void onMessageReceived(const FireWeaponMessage& msg, MainFamily& e) {
    shoot(e);
  }

  void shoot(MainFamily& e) {
    if (e.cooldown.timeLeft > 0) {
      return;
    }
    e.cooldown.timeLeft = e.cooldown.length;

    auto& config = e.weapon.config;

    float inertia = cp::momentForCircle(config.mass, 0, config.radius, cp::Vect(0, 0));
    auto p  = e.transform2D.getGlobalPosition();
    auto r = e.transform2D.getGlobalRotation();

    auto body = std::make_shared<cp::Body>(config.mass, inertia);
    body->setPosition(cp::Vect(p.x, p.y));
    body->setAngle(r.getRadians());
    float y, x;
    r.sincos(y, x);
    body->setVelocity(cp::Vect(x, y) * config.speed);

    auto shape = std::make_shared<cp::CircleShape>(body, config.radius);
    shape->setCollisionType(PROJECTILEBODY);
    shape->setFilter({
      .categories = fPROJECTILE,
      .mask = fASTEROID });

    auto sprite = Sprite()
      .setImage(getResources(), config.projectileImage);

    getWorld().createEntity()
      .addComponent(ProjectileComponent(10, 0, e.weapon.parent))
      .addComponent(BodyComponent(body))
      .addComponent(ShapeComponent(shape))
      .addComponent(Transform2DComponent())
      .addComponent(LifetimeComponent(config.projectileLifetime))
      .addComponent(SpriteComponent(sprite, 0, 1));
  }
};

REGISTER_SYSTEM(WeaponControlSystem)