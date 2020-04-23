#include "game_stage.h"
#include "registry.h"
#include "services/asteroid_service.h"
#include "services/ship_service.h"
#include "services/controller_service.h"
//#include "sprite_layers.h"
#include "game.h"

GameStage::GameStage()
{
}


void GameStage::init()
{
	auto& game = dynamic_cast<SpaceGame&>(getGame());
  auto& res = getResources();
	
	painterService = std::make_shared<PainterService>();

	world = createWorld("stages/game_stage", createSystem, createComponent);
	world->addService(painterService);
	world->addService(game.getInputService());
  world->addService(game.getControllerService());
  auto asteroidService = std::make_shared<AsteroidService>(res.get<ConfigFile>("gameplay/celestial_bodies")->getRoot());
  world->addService(asteroidService);
  auto shipService = std::make_shared<ShipService>(res.get<ConfigFile>("gameplay/ships")->getRoot());
  world->addService(shipService);
}


void GameStage::onFixedUpdate(Time t)
{
	dynamic_cast<SpaceGame&>(getGame()).getInputService()->update(t);
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