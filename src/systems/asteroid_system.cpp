#include "systems/asteroid_system.h"
#include "components/sprite_component.h"
#include "components/health_component.h"
#include "components/body_component.h"
#include "components/shape_component.h"
#include "halley/src/engine/entity/include/halley/entity/components/transform_2d_component.h"
#include "src/utils.h"

class AsteroidSystem final : public AsteroidSystemBase<AsteroidSystem> {
public:
  void init() {
    /*
    spawnAsteroid(getAsteroidService().any());
    spawnAsteroid(getAsteroidService().any());
    spawnAsteroid(getAsteroidService().any());
    spawnAsteroid(getAsteroidService().any());
    spawnAsteroid(getAsteroidService().any());
    spawnAsteroid(getAsteroidService().any());
    spawnAsteroid(getAsteroidService().any());
    spawnAsteroid(getAsteroidService().any());
    spawnAsteroid(getAsteroidService().any());
     */
  }

  void update(Halley::Time t) {

  }

  void spawnAsteroid(const AsteroidConfig& config) {
    cp::Float moment = cp::momentForCircle(config.mass, 0, config.radius);
    auto body = std::make_shared<cp::Body>(config.mass, moment);
    auto pos = cp::Vect((rand() % 1500)-750, (rand() % 1500)-750);
    body->setPosition(pos);
    auto shape = std::make_shared<cp::CircleShape>(body, config.radius);
    shape->setFilter({
      .categories = fASTEROID,
      .mask = fTEAM0SHIP | fTEAM1SHIP | fPROJECTILE | fDETECTOR });
    shape->setCollisionType(ASTEROIDBODY);
    getWorld().createEntity()
      .addComponent(HealthComponent(10))
      .addComponent(BodyComponent(body))
      .addComponent(ShapeComponent(shape))
      .addComponent(Transform2DComponent())
      .addComponent(SpriteComponent(Sprite()
        .setImage(getResources(), config.image)
        .setPivot(Vector2f(.5f, .5f))
        .scaleTo(Vector2f(2*config.radius, 2*config.radius)),
        0, 1));
  }
};

REGISTER_SYSTEM(AsteroidSystem)