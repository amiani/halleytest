#include "systems/physics_system.h"
#include "chipmunk.hpp"

class PhysicsSystem final : public PhysicsSystemBase<PhysicsSystem> {
public:
  void onEntitiesAdded(Halley::Span<BodiesFamily> es) {
    for (auto &e : es) {
      space.add(e.body.body);
      space.add(e.shape.shape);
    }
  }

  void onEntitiesRemoved(Halley::Span<BodiesFamily> es) {
    for (auto& e : es) {
      space.remove(e.body.body);
      space.remove(e.shape.shape);
    }
  }

  void update(Halley::Time time) {
    space.step(time);
  }

private:
  cp::Space space;
};

REGISTER_SYSTEM(PhysicsSystem)