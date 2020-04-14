#include "systems/control_system.h"
#include "chipmunk.hpp"
#include "src/game_stage.h"

class ControlSystem final : public ControlSystemBase<ControlSystem> {
public:
  void update(Halley::Time time, MainFamily& e) {
    if (frames % 8 == 0) {
      if (frames >= 500) terminal = true;
      if (terminal) {
        updateController(time, e, true);
        getAPI().core->setStage(std::make_unique<GameStage>());
      } else {
        auto& action = updateController(time, e, false);
        applyAction(e, action);
      }
    }
    frames++;
  }

  Observation makeObservation(MainFamily& e, bool isTerminal) {
    auto o = Observation();
    o.terminal = isTerminal;
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

  const Action& updateController(Halley::Time time, MainFamily& e, bool isTerminal) {
    auto body = e.body.body;
    if (e.observer.hasValue()) {
      auto observation = makeObservation(e, isTerminal);
      auto distanceToGoal = cp::Vect::dist(body->getPosition(), e.goal.position);
      auto distanceReward = - (10.f / 2200) * distanceToGoal;
      auto reward = e.observer->reward + distanceReward - 1;
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
  int frames = 0;
};
REGISTER_SYSTEM(ControlSystem)