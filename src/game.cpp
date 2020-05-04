#include "game.h"
#include "game_stage.h"
//#include "title_stage.h"
#include "chipmunk.hpp"
#include "torch/torch.h"

void initOpenGLPlugin(IPluginRegistry &registry);
void initSDLSystemPlugin(IPluginRegistry &registry, Maybe<String> cryptKey);
void initSDLAudioPlugin(IPluginRegistry &registry);
void initSDLInputPlugin(IPluginRegistry &registry);
void initAsioPlugin(IPluginRegistry &registry);

void SpaceGame::init(const Environment& env, const Vector<String>& args) {
  if (!args.empty()) {
    fps = std::stoi(args[0]);
    std::istringstream(args[1]) >> loadFromDisk;
    std::istringstream(args[2]) >> train;
  }
}

int SpaceGame::initPlugins(IPluginRegistry& registry)
{
  if (fps > 0) {
    initOpenGLPlugin(registry);
    initSDLAudioPlugin(registry);
    initSDLInputPlugin(registry);
  }
	initSDLSystemPlugin(registry, {});

#ifdef WITH_ASIO
	initAsioPlugin(registry);
#endif

	return HalleyAPIFlags::Video | HalleyAPIFlags::Audio | HalleyAPIFlags::Input | HalleyAPIFlags::Network | HalleyAPIFlags::Platform;
}

void SpaceGame::initResourceLocator(const Path& gamePath, const Path& assetsPath, const Path& unpackedAssetsPath, ResourceLocator& locator) {
	constexpr bool localAssets = Debug::isDebug();
	if (true) { //if (localAssets)
		locator.addFileSystem(unpackedAssetsPath);
	} else {
		const String packs[] = { "music.dat", "sfx.dat", "gameplay.dat", "images.dat", "shaders.dat", "ui.dat", "config.dat" };
		for (auto& pack: packs) {
			locator.addPack(Path(assetsPath) / pack);
		}
	}
}

String SpaceGame::getName() const
{
	return "HalleyTest";
}

String SpaceGame::getDataPath() const
{
	return "Halley/halleytest";
}

bool SpaceGame::isDevMode() const
{
	return true;
}

std::unique_ptr<Stage> SpaceGame::startGame(const HalleyAPI* api)
{
	this->api = api;

	bool vsync = true;

	auto screenSize = api->system->getScreenSize(0);
	auto windowSize = Vector2i(1920, 1080);
	auto windowType = WindowType::BorderlessWindow;
	if (screenSize != windowSize || Debug::isDebug()) {
		windowSize = Vector2i(1920, 1080) / 5 * 4;
		windowType = WindowType::Window;
	}
	
	//zoom = std::max(float(windowSize.x) / 384.0f, float(windowSize.y) / 216.0f);
	
	api->video->setWindow(WindowDefinition(windowType, windowSize, "SpaceRL"));
	api->video->setVsync(vsync);
	api->audio->startPlayback();

	api->audio->setMasterVolume(0.6f);
	api->audio->setGroupVolume("music", 1.0f);
	api->audio->setListener(AudioListenerData(Vector3f(192, 108, -20), 200));

	inputService = std::make_shared<InputService>(*api->input);
	controllerService = std::make_shared<ControllerService>(loadFromDisk, train);

	return std::make_unique<GameStage>();
}

std::shared_ptr<InputService> SpaceGame::getInputService() const
{
	return inputService;
}

std::shared_ptr<ControllerService> SpaceGame::getControllerService() const {
  return controllerService;
}

float SpaceGame::getZoom() const
{
	return zoom;
}

bool SpaceGame::shouldCreateSeparateConsole() const
{
	return Debug::isDebug();
}

HalleyGame(SpaceGame);
