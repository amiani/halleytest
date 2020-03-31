#include <systems/sprite_render_system.h>

using namespace Halley;

class SpriteRenderSystem final : public SpriteRenderSystemBase<SpriteRenderSystem> {
public:	
	void render(RenderContext& rc)
	{
		for (auto& e: spriteFamily) {
			auto& sprite = e.sprite.sprite;
			if (sprite.hasMaterial()) {
        auto p = e.transform2D.getGlobalPosition();
				sprite.setPos(p);
        sprite.setRotation(e.transform2D.getGlobalRotation());
				getPainterService().spritePainter.add(sprite, e.sprite.mask, e.sprite.layer, p.y);
			}
		}
		for (auto& e: textFamily) {
			auto& text = e.textLabel.text;
			text.setPosition(e.transform2D.getGlobalPosition());
			getPainterService().spritePainter.add(text, e.textLabel.mask, e.textLabel.layer, text.getPosition().y);
		}
	}
};

REGISTER_SYSTEM(SpriteRenderSystem)