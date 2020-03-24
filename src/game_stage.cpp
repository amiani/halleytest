#include "game_stage.h"
#include "registry.h"
#include "components/sprite_component.h"
#include "components/position_component.h"
#include "components/camera_component.h"
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
	rhythmService = std::make_shared<RhythmService>();

	world = createWorld("stages/game_stage", createSystem, createComponent);
	world->addService(painterService);
	world->addService(game.getInputService());

	world->createEntity()
		.addComponent(PositionComponent(Vector2f(192.0f, 108.0f)))
		.addComponent(CameraComponent(game.getZoom(), Colour4f(0.0f, 0.0f, 0.0f), 1, 0));
}

void GameStage::onVariableUpdate(Time t)
{
	dynamic_cast<HalleyTestGame&>(getGame()).getInputService()->update(t);
	world->step(TimeLine::VariableUpdate, t);
}

void GameStage::onRender(RenderContext& rc) const
{
	painterService->spritePainter.start(300);
	world->render(rc);
}
