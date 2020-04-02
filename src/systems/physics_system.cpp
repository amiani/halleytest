#include "systems/physics_system.h"
#include "chipmunk.hpp"
#include "src/utils.h"

class PhysicsSystem final : public PhysicsSystemBase<PhysicsSystem> {
public:
  void init() {
    space.setDamping(.6);

    space.addBeginCollisionHandler(PROJECTILE, [this](cp::Arbiter arb, auto& s) {
      auto laser = reinterpret_cast<Halley::Entity*>(arb.getBodyA().getUserData());
      auto other = reinterpret_cast<Halley::Entity*>(arb.getBodyB().getUserData());
      getWorld().destroyEntity(laser->getEntityId());
      sendMessage(other->getEntityId(), HitMessage(10));
      return false;
    });

    space.addBeginCollisionHandler(SENSOR, [this](cp::Arbiter arb, auto& s) {
      auto sensor = reinterpret_cast<Halley::Entity*>(arb.getBodyA().getUserData());
      auto other = reinterpret_cast<Halley::Entity*>(arb.getBodyB().getUserData());
      auto& sensorComp = sensor->getComponent<SensorComponent>();
      sensorComp.entities.push_back(other);
      return false;
    });

    space.addSeparateCollisionHandler(SENSOR, [this](cp::Arbiter arb, auto& s) {
      auto sensor = reinterpret_cast<Halley::Entity*>(arb.getBodyA().getUserData());
      auto other = reinterpret_cast<Halley::Entity*>(arb.getBodyB().getUserData());
      auto& es = sensor->getComponent<SensorComponent>().entities;
      es.erase(std::remove(es.begin(), es.end(), other), es.end());
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

  void onEntitiesAdded(Halley::Span<PhysicalFamily> es) {
    for (auto& e : es) {
      auto entity = getWorld().tryGetEntity(e.entityId);
      e.body.body->setUserData(reinterpret_cast<cp::DataPointer>(entity));
      bodiesToAdd.push_back(e.body.body);
      shapesToAdd.push_back(e.shape.shape);
      if (e.sensor.hasValue()) {
        shapesToAdd.push_back(e.sensor->shape);
      }
    }
  }

  void onEntitiesRemoved(Halley::Span<PhysicalFamily> es) {
    for (auto& e : es) {
      bodiesToRemove.push_back(e.body.body);
      shapesToRemove.push_back(e.shape.shape);
      if (e.sensor.hasValue()) {
        shapesToRemove.push_back(e.sensor->shape);
      }
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