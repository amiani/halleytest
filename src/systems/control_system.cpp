#include "systems/control_system.h"
#include "chipmunk.hpp"
#include "src/game_stage.h"

class ControlSystem final : public ControlSystemBase<ControlSystem> {
public:
  void update(Halley::Time time, MainFamily& e) {
    if (terminal) {
      updateController(time, e);
      getAPI().core->setStage(std::make_unique<GameStage>());
    } else {
      auto& action = updateController(time, e);
      applyAction(e, action);
    }
  }

  Observation makeObservation(MainFamily& e) {
    auto o = Observation();
    o.self = makeEntityData(*e.body.body, e.health.health);
    for (auto& other : e.detector.entities) {
      if (other->isAlive()) {
        auto& bodyComponent = other->getComponent<BodyComponent>();
        auto& body = bodyComponent.body;
        //auto& health = other->getComponent<HealthComponent>().health;
        o.detectedBodies.push_back(makeEntityData(*body, 10));
      }
    }
    return o;
  }

  EntityData makeEntityData(cp::Body& body, int health) {
    auto e = (Halley::Entity*)body.getUserData();
    auto eId = e->getEntityId().toString();
    return {
      .position = body.getPosition(),
      .rotation = body.getAngle(),
      .velocity = body.getVelocity(),
      .health = health
    };
  }

  const Action& updateController(Halley::Time time, MainFamily& e) {
    auto body = e.body.body;
    if (e.observer.hasValue()) {
      auto observation = makeObservation(e);
      float reward = e.observer->reward;
      e.observer->reward = 0;
      return e.shipControl.controller->update(time, observation, reward);
    } else {
      return e.shipControl.controller->update(time);
    }
  }

  void applyAction(MainFamily& e, const Action& a) {
    auto& body = e.body.body;
    if (a.throttle) {
      body->applyForceAtLocalPoint(cp::Vect(500, 0), cp::Vect(0, 0));
    }

    cp::Vect bodyPos = body->getPosition();
    double angle = cp::Vect::toAngle(a.target - bodyPos);
    body->setAngle(angle);

    if (a.fire && !e.hardpoints.hardpoints.empty()) {
      sendMessage(e.hardpoints.hardpoints.front().weaponId, FireWeaponMessage());
    }
  }

  void onMessageReceived(const ReachedGoalMessage& msg, MainFamily& e) {
    terminal = true;
  }

private:
  bool terminal = false;
};
REGISTER_SYSTEM(ControlSystem)