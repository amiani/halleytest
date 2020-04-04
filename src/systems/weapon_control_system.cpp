#include "systems/weapon_control_system.h"
#include "components/body_component.h"
#include "components/shape_component.h"
#include "components/sprite_component.h"
#include "halley/src/engine/entity/include/halley/entity/components/transform_2d_component.h"
#include "chipmunk.hpp"
#include "src/utils.h"

class WeaponControlSystem : public WeaponControlSystemBase<WeaponControlSystem> {
public:
  void update(Time t, MainFamily& e) {
  }

  void onMessageReceived(const FireWeaponMessage& msg, MainFamily& e) {
    shoot(e);
  }

  void shoot(MainFamily& e) {
    float mass = e.weapon.config.mass;
    float radius = e.weapon.config.radius;
    float inertia = cp::momentForCircle(mass, 0, radius, cp::Vect(0, 0));
    auto p  = e.transform2D.getGlobalPosition();
    auto r = e.transform2D.getGlobalRotation();

    auto body = std::make_shared<cp::Body>(mass, inertia);
    body->setPosition(cp::Vect(p.x, p.y));
    body->setAngle(r.getRadians());
    float y, x;
    r.sincos(y, x);
    body->setVelocity(cp::Vect(x, y) * 500);

    auto shape = std::make_shared<cp::CircleShape>(body, radius);
    shape->setCollisionType(PROJECTILEBODY);
    shape->setFilter({
      .categories = PLAYERPROJECTILE,
      .mask = ASTEROID });

    auto sprite = Sprite()
      .setImage(getResources(), e.weapon.config.projectileImage);

    getWorld().createEntity()
      .addComponent(BodyComponent(body))
      .addComponent(ShapeComponent(shape))
      .addComponent(Transform2DComponent())
      .addComponent(SpriteComponent(sprite, 0, 1));
  }
};

REGISTER_SYSTEM(WeaponControlSystem)