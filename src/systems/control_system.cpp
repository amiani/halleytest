#include "systems/control_system.h"
#include "chipmunk.hpp"
#include "src/game_stage.h"
#include "src/control/action.h"

class ControlSystem final : public ControlSystemBase<ControlSystem> {
public:
  Action lastAction;
  void update(Halley::Time time, MainFamily& e) {
    if (frames % 8 == 0) {
      if (frames >= 180) terminal = true;
      auto action = updateController(time, e, terminal);
      if (terminal) {
        getAPI().core->setStage(std::make_unique<GameStage>());
      } else {
        lastAction = action;
      }
    }
    applyAction(e, lastAction);
    frames++;
  }

  Observation makeObservation(MainFamily& e, bool isTerminal) {
    auto o = Observation();
    o.terminal = isTerminal;
    o.self = makeEntityData(*e.body.body, e.health.health);
    for (auto& id : e.detector.entities) {
      auto other = getWorld().tryGetEntity(id);
      if (other->isAlive()) {
        auto& bodyComponent = other->getComponent<BodyComponent>();
        auto& body = bodyComponent.body;
        //auto& health = other->getComponent<HealthComponent>().health;
        o.detectedBodies.push_back(makeEntityData(*body, 10));
      }
    }
    o.goal = e.goal.position;
    o.angularVelocity = e.body.body->getAngularVelocity();
    o.uuid = e.shipControl.controller->getUUID();
    return o;
  }

  EntityData makeEntityData(cp::Body& body, int health) {
    return {
      .position = body.getPosition(),
      .rotation = body.getAngle(),
      .velocity = body.getVelocity(),
      .health = health
    };
  }

  double lastDistance = -1;
  Action updateController(Halley::Time time, MainFamily& e, bool isTerminal) {
    auto body = e.body.body;
    if (e.observer.hasValue()) {
      auto observation = makeObservation(e, isTerminal);
      auto distanceToGoal = cp::Vect::dist(body->getPosition(), e.goal.position);
      if (lastDistance == -1) lastDistance = distanceToGoal;
      auto delta = lastDistance - distanceToGoal;
      //auto distanceReward = -(1.f/1000) * distanceToGoal;
      //auto reward = e.observer->reward + distanceReward;
      float reward = 0;
      if (reachedGoal) reward += 1;
      if (delta > 7) {
        reward += 1;
      } else if (delta < -7) {
        reward -= 1;
      }
      lastDistance = distanceToGoal;
      e.observer->reward = 0;
      e.observer->totalReward += reward;
      return e.shipControl.controller->update(time, observation, reward);
    } else {
      return e.shipControl.controller->update(time);
    }
  }

  void applyAction(MainFamily& e, const Action& a) {
    auto& body = e.body.body;
    if (a.throttle) {
      body->applyForceAtLocalPoint(cp::Vect(200, 0), cp::Vect(0, 0));
    }
    if (a.direction == LEFT) {
      body->setTorque(-10000);
    } else if (a.direction == RIGHT) {
      body->setTorque(10000);
    } else {
      auto damping = -5000*body->getAngularVelocity();
      body->setTorque(damping);
    }

    if (a.fire && !e.hardpoints.hardpoints.empty()) {
      sendMessage(e.hardpoints.hardpoints.front().weaponId, FireWeaponMessage());
    }
  }

  void onMessageReceived(const ReachedGoalMessage& msg, MainFamily& e) {
    terminal = true;
    reachedGoal = true;
  }

private:
  bool reachedGoal = false;
  bool terminal = false;
  unsigned long long frames = 0;
};

REGISTER_SYSTEM(ControlSystem)
