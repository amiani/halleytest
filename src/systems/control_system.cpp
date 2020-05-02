#include "components/team_component.h"
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
    Observation o(SelfState(*e.body.body, e.health.health));
    o.terminal = isTerminal;
    for (auto& id : e.detector.entities) {
      auto other = getWorld().tryGetEntity(id);
      if (other && other->isAlive()) {
        auto& bodyComponent = other->getComponent<BodyComponent>();
        auto& body = *bodyComponent.body;
        auto teamComponent = other->tryGetComponent<TeamComponent>();
        if (teamComponent && teamComponent->team != e.team.team) {
          o.enemies.emplace_back(body, 100, ENEMY);
        }
        //auto health = other->getComponent<HealthComponent>().health;
        o.detectedBodies.emplace_back(body, 10, NEUTRAL);
      }
    }
    o.uuid = e.shipControl.controller->getUUID();
    return o;
  }

  Action updateController(Halley::Time time, MainFamily& e, bool isTerminal) {
    auto body = e.body.body;
    if (e.observer.hasValue()) {
      auto observation = makeObservation(e, isTerminal);
      return e.shipControl.controller->update(time, observation, 0);
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

  void onMessageReceived(const HitMessage& msg, MainFamily& e) {
    e.observer->reward += msg.kinetic + msg.energy;
  }

private:
  bool reachedGoal = false;
  bool terminal = false;
  unsigned long long frames = 0;
};

REGISTER_SYSTEM(ControlSystem)
