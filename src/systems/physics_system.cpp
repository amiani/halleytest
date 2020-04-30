#include <any>
#include "systems/physics_system.h"
#include "chipmunk.hpp"
#include "src/utils.h"

class PhysicsSystem final : public PhysicsSystemBase<PhysicsSystem> {
public:
  void init() {
    addCollisionHandlers();
    addBoundaries();
  }

  void update(Halley::Time time) {
    space.step(time);
    updateChipmunkObjects();

    for (auto& e : bodiesFamily) {
      e.transform2D.setGlobalPosition(chipToHalley(e.body.body->getPosition()));
      e.transform2D.setGlobalRotation(e.body.body->getAngle());
    }

    for (auto& e : lifetimeFamily) {
      e.lifetime.timeLeft -= time;
      if (e.lifetime.timeLeft <= 0) {
        getWorld().destroyEntity(e.entityId);
      }
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

  void onEntitiesAdded(Halley::Span<PhysicalFamily> es) {
    for (auto& e : es) {
      e.body.body->userData = e.entityId;
      bodiesToAdd.push_back(e.body.body);
      shapesToAdd.push_back(e.shape.shape);
      if (e.detector.hasValue()) {
        shapesToAdd.push_back(e.detector->shape);
      }
    }
  }

  void onEntitiesRemoved(Halley::Span<PhysicalFamily> es) {
    for (auto& e : es) {
      bodiesToRemove.push_back(e.body.body);
      shapesToRemove.push_back(e.shape.shape);
      if (e.detector.hasValue()) {
        shapesToRemove.push_back(e.detector->shape);
      }
    }
  }

  void addCollisionHandlers() {
    space.addBeginCollisionHandler(PROJECTILEBODY, [this](cp::Arbiter arb, cp::Space& s) {
      auto laser = std::any_cast<Halley::EntityId>(arb.getBodyA().userData);
      auto other = std::any_cast<Halley::EntityId>(arb.getBodyB().userData);
      getWorld().destroyEntity(laser);
      sendMessage(other, HitMessage(10));
      return false;
    });

    space.addBeginCollisionHandler(DETECTORBODY, [this](cp::Arbiter arb, auto& s) {
      auto detector = getEntity(arb.getBodyA());
      auto otherId = std::any_cast<Halley::EntityId>(arb.getBodyB().userData);
      auto& detectorComp = detector->getComponent<DetectorComponent>();
      detectorComp.entities.push_back(otherId);
      return false;
    });

    space.addSeparateCollisionHandler(DETECTORBODY, [this](cp::Arbiter arb, auto& s) {
      auto detector = getEntity(arb.getBodyA());
      auto otherId = std::any_cast<Halley::EntityId>(arb.getBodyB().userData);
      auto& es = detector->getComponent<DetectorComponent>().entities;
      es.erase(std::remove(es.begin(), es.end(), otherId), es.end());
    });

    space.addBeginCollisionHandler(GOALBODY, PLAYERSHIPBODY, [this](cp::Arbiter arb, auto& s) {
      auto playerId = std::any_cast<Halley::EntityId>(arb.getBodyB().userData);
      sendMessage(playerId, ReachedGoalMessage());
      std::cout << "reached goal!!!\n";
      return false;
    });

    space.addPostSolveCollisionHandler(PLAYERSHIPBODY, [this](cp::Arbiter arb, auto& s) {
      auto playerId = std::any_cast<Halley::EntityId>(arb.getBodyA().userData);
      auto ke = arb.totalKineticEnergy();
      sendMessage(playerId, ImpactMessage(ke));
    });
  }

  Halley::Entity* getEntity(const cp::Body& body) {
    auto id = std::any_cast<Halley::EntityId>(body.userData);
    return getWorld().tryGetEntity(id);
  }

  void addBoundaries() {
    auto halfx = 1920;
    auto halfy = 1080;
    addBoundary(cp::Vect(0, -halfy), cp::Vect(-halfx, 0), cp::Vect(halfx, 0));
    addBoundary(cp::Vect(-halfx, 0), cp::Vect(0, -halfy), cp::Vect(0, halfy));
    addBoundary(cp::Vect(0, halfy), cp::Vect(-halfx, 0), cp::Vect(halfx, 0));
    addBoundary(cp::Vect(halfx, 0), cp::Vect(0, -halfy), cp::Vect(0, halfy));
  }

  void addBoundary(cp::Vect pos, cp::Vect a, cp::Vect b) {
    auto& world = getWorld();
    auto body = std::make_shared<cp::StaticBody>();
    body->setPosition(pos);
    auto shape = std::make_shared<cp::SegmentShape>(body, a, b, 10);
    shape->setFilter({ .categories = BOUNDARY });
    world.createEntity()
      .addComponent(BodyComponent(body))
      .addComponent(ShapeComponent(shape));
  }

private:
  cp::Space space;
  std::vector<std::shared_ptr<cp::Body>> bodiesToAdd;
  std::vector<std::shared_ptr<cp::Body>> bodiesToRemove;
  std::vector<std::shared_ptr<cp::Shape>> shapesToAdd;
  std::vector<std::shared_ptr<cp::Shape>> shapesToRemove;
};

REGISTER_SYSTEM(PhysicsSystem)