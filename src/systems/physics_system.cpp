#include "systems/physics_system.h"
#include "chipmunk.hpp"

class PhysicsSystem final : public PhysicsSystemBase<PhysicsSystem> {
public:
    void init() {
    }

    void onEntitiesAdded(Halley::Span<BodiesFamily> es) {
      for (auto &e : es) {
        space.add(e.body.body);
      }
    }

    void onEntitiesRemoved(Halley::Span<BodiesFamily> es) {
      for (auto& e : es) {
      }
    }

    void update(Halley::Time time) {
    }

private:
    cp::Space space;
};

REGISTER_SYSTEM(PhysicsSystem)