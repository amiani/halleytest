#include <gen/cpp/components/shape_component.h>
#include "game_stage.h"
#include "registry.h"
#include "components/sprite_component.h"
#include "components/position_component.h" 
#include "components/background_camera_component.h"
#include "components/body_component.h"
#include "components/shape_component.h"
#include "components/device_control_component.h"
#include "services/input_service.h"
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

  for (auto& n : getResources().get<ConfigFile>("gameplay/celestial_bodies")->getRoot()["asteroids"].asSequence()) {
    const auto id = n["id"].asString();
    const auto mass = n["mass"].asFloat();
    const auto radius = n["radius"].asFloat();
    const auto image = n["image"].asString();
    const cp::Float moment = cp::momentForCircle(mass, 0, radius);
    const auto body = std::make_shared<cp::Body>(mass, moment);
    body->setPosition(cp::Vect(0, 100));
    world->createEntity()
      .addComponent(BodyComponent(body))
      .addComponent(ShapeComponent(std::make_shared<cp::CircleShape>(body, radius)))
      .addComponent(SpriteComponent(Sprite()
        .setImage(getResources(), image)
        .setPivot(Vector2f(.5f, .5f))
        .scaleTo(Vector2f(2*radius, 2*radius)),
        0, 1));
  }

  const cp::Float mass = 1;
  const cp::Float radius = 32;
  const cp::Float moment = cp::momentForCircle(mass, 0, radius);
  const auto body = std::make_shared<cp::Body>(mass, moment);
  const auto sprite = Sprite()
    .setImage(getResources(), "large_grey_01.png")
    .setPivot(Vector2f(.5f, .5f));
  const auto bgSprite = Sprite().setImage(getResources(), "goldstartile.jpg");
	world->createEntity()
	  .addComponent(BodyComponent(body))
	  .addComponent(ShapeComponent(std::make_shared<cp::CircleShape>(body, radius)))
	  .addComponent(SpriteComponent(sprite, 0, 1))
    .addComponent(DeviceControlComponent(&game.getInputService()->getInput()))
		.addComponent(BackgroundCameraComponent(game.getZoom(), Colour4f(0.0f, 0.0f, 0.0f), 1, 0, bgSprite));
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
