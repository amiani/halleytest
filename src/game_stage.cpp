#include <gen/cpp/components/shape_component.h>
#include "game_stage.h"
#include "registry.h"
#include "components/sprite_component.h"
#include "components/health_component.h"
#include "components/background_camera_component.h"
#include "components/body_component.h"
#include "components/sensor_component.h"
#include "components/hardpoints_component.h"
#include "components/shape_component.h"
#include "halley/src/engine/entity/include/halley/entity/components/transform_2d_component.h"
#include "components/weapon_component.h"
#include "components/parent_component.h"
#include "components/ship_control_component.h"
#include "components/weapon_control_component.h"
#include "services/input_service.h"
#include "services/controller_service.h"
#include "src/hardpoint.h"
//#include "sprite_layers.h"
#include "game.h"
#include "title_stage.h"
#include "src/utils.h"

GameStage::GameStage()
{
}


void GameStage::init()
{
	auto& game = dynamic_cast<HalleyTestGame&>(getGame());
	
	painterService = std::make_shared<PainterService>();

	world = createWorld("stages/game_stage", createSystem, createComponent);
	world->addService(painterService);
	world->addService(game.getInputService());
  auto controllerService = std::make_shared<ControllerService>();
  world->addService(controllerService);


  auto& laserNode = getResources().get<ConfigFile>("gameplay/weapons")->getRoot()["weapons"].asSequence()[0];
  auto laserConfig = WeaponConfig();
  laserConfig.load(laserNode);
  auto laser = world->createEntity()
    .addComponent(WeaponControlComponent())
    .addComponent(WeaponComponent(laserConfig));
  //std::cout << "weaponId: " << laser.getEntityId().value << std::endl;

  cp::Float mass = 1;
  cp::Float radius = 32;
  cp::Float moment = cp::momentForCircle(mass, 0, radius);
  auto body = std::make_shared<cp::Body>(mass, moment);
  auto shape = std::make_shared<cp::CircleShape>(body, radius);
  shape->setFilter({ .categories = 0b1, .mask = 0b11100 });
  shape->setCollisionType(PLAYERSHIP);
  auto sensor = std::make_shared<cp::CircleShape>(body, 1920/2);
  sensor->setFilter({ .categories = 0b100000, .mask = 0b10000 });
  sensor->setCollisionType(SENSOR);
  sensor->setSensor(true);
  auto sprite = Sprite()
    .setImage(getResources(), "large_grey_01.png")
    .setPivot(Vector2f(.5f, .5f));
  auto bgSprite = Sprite().setImage(getResources(), "goldstartile.jpg");
  auto device = game.getInputService()->getInput();
  auto h = Hardpoint();
  h.id = laser.getEntityId();
  h.offset = Vector2f(25, 0);
	auto ship = world->createEntity()
	  .addComponent(BodyComponent(body))
	  .addComponent(ShapeComponent(shape))
    .addComponent(SensorComponent(sensor, std::vector<Entity*>()))
	  .addComponent(SpriteComponent(sprite, 0, 1))
    .addComponent(HardpointsComponent(std::vector<Hardpoint>{h}))
    .addComponent(Transform2DComponent())
    .addComponent(ShipControlComponent(controllerService->makeInputController(device)))
		.addComponent(BackgroundCameraComponent(game.getZoom(), Colour4f(0.0f, 0.0f, 0.0f), 1, 0, bgSprite));
  //std::cout << "shipId: " << ship.getEntityId().value << std::endl;

  laser.addComponent(Transform2DComponent(ship.getComponent<Transform2DComponent>(), Vector2f(25, 0)));
  
  auto& n = getResources().get<ConfigFile>("gameplay/celestial_bodies")->getRoot()["asteroids"].asSequence()[0];
  auto id = n["id"].asString();
  auto astMass = n["mass"].asFloat();
  auto astRadius = n["radius"].asFloat();
  auto image = n["image"].asString();
  cp::Float astMoment = cp::momentForCircle(astMass, 0, astRadius);
  auto astBody = std::make_shared<cp::Body>(astMass, astMoment);
  astBody->setPosition(cp::Vect(0, 100));
  auto astShape = std::make_shared<cp::CircleShape>(astBody, astRadius);
  astShape->setFilter({ .categories = 0b10000, .mask = 0b111111 });
  astShape->setCollisionType(ASTEROID);
  auto asteroid = world->createEntity()
    .addComponent(HealthComponent(10))
    .addComponent(BodyComponent(astBody))
    .addComponent(ShapeComponent(astShape))
    .addComponent(Transform2DComponent())
    .addComponent(SpriteComponent(Sprite()
      .setImage(getResources(), image)
      .setPivot(Vector2f(.5f, .5f))
      .scaleTo(Vector2f(2*astRadius, 2*astRadius)),
      0, 1));
  //std::cout << "astId: " << asteroid.getEntityId().value << std::endl;
}


void GameStage::onFixedUpdate(Time t)
{
	dynamic_cast<HalleyTestGame&>(getGame()).getInputService()->update(t);
  world->step(TimeLine::FixedUpdate, t);
}

void GameStage::onVariableUpdate(Time t)
{
	world->step(TimeLine::VariableUpdate, t);
}

void GameStage::onRender(RenderContext& rc) const
{
	painterService->spritePainter.start(300);
	world->render(rc);
}