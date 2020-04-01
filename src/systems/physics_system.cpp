#include "systems/physics_system.h"
#include "chipmunk.hpp"
#include "src/utils.h"

class PhysicsSystem final : public PhysicsSystemBase<PhysicsSystem> {
public:
  void init() {
    space.setDamping(.6);
    /*
    space.addBeginCollisionHandler(cp::CollisionType(0b10000), cp::CollisionType(0b10), [this](cp::Arbiter arb, auto& s) {
      auto asteroidId = static_cast<Halley::EntityId*>(arb.getBodyA().getUserData());
      auto laserId = static_cast<Halley::EntityId*>(arb.getBodyB().getUserData());


      sendMessage(*asteroidId, HitMessage(10));
      arb.callWildcardBeginA();
      return false;
    });
    */
    space.addBeginCollisionHandler(1, [this](cp::Arbiter arb, auto& s) {
      auto laser = reinterpret_cast<Halley::Entity*>(arb.getBodyA().getUserData());
      auto other = reinterpret_cast<Halley::Entity*>(arb.getBodyB().getUserData());
      getWorld().destroyEntity(laser->getEntityId());
      sendMessage(other->getEntityId(), HitMessage(10));
      return false;
    });
  }

  void update(Halley::Time time) {
    space.step(time);
    updateChipmunkObjects();

    for (auto& e : bodiesFamily) {
      e.transform2D.setGlobalPosition(chipToHalley(e.body.body->getPosition()));
      e.transform2D.setGlobalRotation(e.body.body->getAngle());
    }
  }

  void updateChipmunkObjects() {
    while (!bodiesToAdd.empty()) {
      space.add(bodiesToAdd.back());
      bodiesToAdd.pop_back();
    }
    while (!bodiesToRemove.empty()) {
      space.remove(bodiesToRemove.back());
      bodiesToRemove.pop_back();
    }
    while (!shapesToAdd.empty()) {
      space.add(shapesToAdd.back());
      shapesToAdd.pop_back();
    }
    while (!shapesToRemove.empty()) {
      space.remove(shapesToRemove.back());
      shapesToRemove.pop_back();
    }
  }

  void onEntitiesAdded(Halley::Span<ShapesFamily> es) {
    for (auto& e : es) {
      auto entity = getWorld().tryGetEntity(e.entityId);
      e.body.body->setUserData(reinterpret_cast<cp::DataPointer>(entity));
      bodiesToAdd.push_back(e.body.body);
      shapesToAdd.push_back(e.shape.shape);
    }
  }

  void onEntitiesRemoved(Halley::Span<ShapesFamily> es) {
    for (auto& e : es) {
      bodiesToRemove.push_back(e.body.body);
      shapesToRemove.push_back(e.shape.shape);
    }
  }

private:
  cp::Space space;
  std::vector<std::shared_ptr<cp::Body>> bodiesToAdd;
  std::vector<std::shared_ptr<cp::Body>> bodiesToRemove;
  std::vector<std::shared_ptr<cp::Shape>> shapesToAdd;
  std::vector<std::shared_ptr<cp::Shape>> shapesToRemove;
};

REGISTER_SYSTEM(PhysicsSystem)