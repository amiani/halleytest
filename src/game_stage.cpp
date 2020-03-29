#include <gen/cpp/components/shape_component.h>
#include "game_stage.h"
#include "registry.h"
#include "components/sprite_component.h"
#include "components/health_component.h"
#include "components/position_component.h" 
#include "components/background_camera_component.h"
#include "components/body_component.h"
#include "components/hardpoints_component.h"
#include "components/shape_component.h"
#include "components/weapon_component.h"
#include "components/parent_component.h"
#include "components/ship_control_component.h"
#include "services/input_service.h"
#include "services/controller_service.h"
//#include "sprite_layers.h"
#include "game.h"
#include "title_stage.h"

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

  for (auto& n : getResources().get<ConfigFile>("gameplay/celestial_bodies")->getRoot()["asteroids"].asSequence()) {
    auto id = n["id"].asString();
    auto mass = n["mass"].asFloat();
    auto radius = n["radius"].asFloat();
    auto image = n["image"].asString();
    cp::Float moment = cp::momentForCircle(mass, 0, radius);
    auto body = std::make_shared<cp::Body>(mass, moment);
    body->setPosition(cp::Vect(0, 100));
    world->createEntity()
      .addComponent(HealthComponent(100))
      .addComponent(BodyComponent(body))
      .addComponent(ShapeComponent(std::make_shared<cp::CircleShape>(body, radius)))
      .addComponent(SpriteComponent(Sprite()
        .setImage(getResources(), image)
        .setPivot(Vector2f(.5f, .5f))
        .scaleTo(Vector2f(2*radius, 2*radius)),
        0, 1));
  }

  cp::Float mass = 1;
  cp::Float radius = 32;
  cp::Float moment = cp::momentForCircle(mass, 0, radius);
  auto body = std::make_shared<cp::Body>(mass, moment);
  auto sprite = Sprite()
    .setImage(getResources(), "large_grey_01.png")
    .setPivot(Vector2f(.5f, .5f));
  auto bgSprite = Sprite().setImage(getResources(), "goldstartile.jpg");
  auto device = game.getInputService()->getInput();
	auto ship = world->createEntity()
	  .addComponent(BodyComponent(body))
	  .addComponent(ShapeComponent(std::make_shared<cp::CircleShape>(body, radius)))
	  .addComponent(SpriteComponent(sprite, 0, 1))
    .addComponent(HardpointsComponent())
    .addComponent(ShipControlComponent(controllerService->makeInputController(device)))
		.addComponent(BackgroundCameraComponent(game.getZoom(), Colour4f(0.0f, 0.0f, 0.0f), 1, 0, bgSprite));

  /*
  auto laserConfig = WeaponConfig();
  auto& laserNode = getResources().get<ConfigFile>("gameplay/weapons")->getRoot()["weapons"]["laser"];
  laserConfig.load(laserNode);
  auto laser = world->createEntity()
    .addComponent(WeaponComponent(laserConfig))
    .addComponent(ParentComponent(&ship, cp::Vect(0, -20)));
  */
  
  //ship.getComponent<HardpointsComponent>().hardpoints.push_back(laser);
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
