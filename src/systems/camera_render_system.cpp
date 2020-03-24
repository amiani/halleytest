#include <systems/camera_render_system.h>

using namespace Halley;

class CameraRenderSystem final : public CameraRenderSystemBase<CameraRenderSystem> {
public:
	void render(RenderContext& rc)
	{
		std::vector<CameraFamily*> cams;
		cams.reserve(cameraFamily.size());
		for (auto& c: cameraFamily) {
			cams.push_back(&c);
		}
		std::sort(cams.begin(), cams.end(), [] (const CameraFamily* a, const CameraFamily* b) -> bool
		{
			return a->camera.layer < b->camera.layer;
		});
		
		for (auto& cPtr: cams) {
			auto& c = *cPtr;
      auto cPos = c.body.body->getPosition();
			auto camera = Camera().setZoom(c.camera.zoom).setPosition(Vector2f(cPos.x, cPos.y));

			rc.with(camera).bind([&] (Painter& painter) {
				if (c.camera.clear) {
					painter.clear(c.camera.clear.value());
				}
				getPainterService().spritePainter.draw(c.camera.mask, painter);
			});
		}
	}
};

REGISTER_SYSTEM(CameraRenderSystem)

