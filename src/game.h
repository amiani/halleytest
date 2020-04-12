#pragma once

#include <halley.hpp>
#include "services/controller_service.h"
#include "services/input_service.h"

using namespace Halley;

class HalleyTestGame : public Game {
public:
	void init(const Environment& env, const Vector<String>& args) override;
	int initPlugins(IPluginRegistry& registry) override;
	void initResourceLocator(const Path& gamePath, const Path& assetsPath, const Path& unpackedAssetsPath, ResourceLocator& locator) override;

	String getName() const override;
	String getDataPath() const override;
	bool isDevMode() const override;
	std::unique_ptr<Stage> startGame(const HalleyAPI* api) override;
  
  int getTargetFPS() const override { return fps; };

	std::shared_ptr<InputService> getInputService() const;
	std::shared_ptr<ControllerService> getControllerService() const;
	float getZoom() const;

	bool shouldCreateSeparateConsole() const override;

private:
	const HalleyAPI* api = nullptr;
	float zoom = 1;
	int fps = 60;
	
	std::shared_ptr<InputService> inputService;
	std::shared_ptr<ControllerService> controllerService;
};
