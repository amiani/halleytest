#include <systems/camera_render_system.h>
#include "src/utils.h"

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
			return a->backgroundCamera.layer < b->backgroundCamera.layer;
		});
		
		for (auto& cPtr: cams) {
			auto& c = *cPtr;
      auto cpos = c.body.body->getPosition();
			auto camera = Camera().setZoom(c.backgroundCamera.zoom).setPosition(Vector2f(cpos.x, -cpos.y));

			rc.with(camera).bind([&] (Painter& painter) {
				if (c.backgroundCamera.clear) {
					painter.clear(c.backgroundCamera.clear.value());
				}
        const auto bg = c.backgroundCamera.background;
        const auto size = bg.getScaledSize();
        const auto viewSize = painter.getViewPort().getSize();
        const Vector2i numTiles = Vector2i(
          ceil(viewSize.x / size.x) + 1,
          ceil(viewSize.y / size.y) + 1);
        for (int x = 0; x <= numTiles.x; x++) {
          for (int y = 0; y <= numTiles.y; y++) {
            const auto posX = cpos.x - (int)(cpos.x) % (int)size.x - viewSize.x / 2 + size.x * (x - 1);
            const auto posY = cpos.y - (int)(cpos.y) % (int)size.y - viewSize.y / 2 + size.y * y;
            Sprite()
              .setPosition(Vector2f(posX, -posY))
              .setMaterial(std::make_shared<Material>(bg.getMaterial()))
              .draw(painter);
          }
        }
				getPainterService().spritePainter.draw(c.backgroundCamera.mask, painter);
			});
		}
	}
};

REGISTER_SYSTEM(CameraRenderSystem)

