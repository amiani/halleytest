#include "components/team_component.h"
#include "systems/control_system.h"
#include "chipmunk.hpp"
#include "src/game_stage.h"
#include "src/control/action.h"

class ControlSystem final : public ControlSystemBase<ControlSystem> {
public:
  void update(Halley::Time time) {
    if (frames >= 3600) limitTerminal = true;
    for (auto& e : mainFamily) {
      if (frames % 8 == 0) {
        updateController(time, e, limitTerminal);
        if (e.health.health <= 0) {
          getWorld().destroyEntity(e.entityId);
        }
      }
      applyAction(e);
    }
    if (limitTerminal) {
      getAPI().core->setStage(std::make_unique<GameStage>());
    }
    ++frames;
  }

  Observation makeObservation(MainFamily& e, bool limitTerminal) {
    auto health = std::max(0, e.health.health);
    Observation o(SelfState(*e.body.body, health));
    o.terminal = limitTerminal || health == 0;
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
        o.detectedBodies.emplace_back(body, 10, ALLY);
      }
    }
    o.uuid = e.shipControl.controller->getUUID();
    return o;
  }

  void updateController(Halley::Time time, MainFamily& e, bool isTerminal) {
    auto body = e.body.body;
    if (e.observer.hasValue()) {
      auto observation = makeObservation(e, isTerminal);
      auto reward = e.observer->reward;
      reward += e.health.health <= 0 ? -1 : 0;
      if (body->getPosition().length() > 1920/2) {
        reward += -.005;
      }
      e.shipControl.lastAction = e.shipControl.controller->update(time, observation, reward);
    } else {
      e.shipControl.lastAction = e.shipControl.controller->update(time);
    }
    e.observer->reward = 0;
  }

  void applyAction(MainFamily& e) {
    auto& a = e.shipControl.lastAction;
    auto& body = e.body.body;
    if (a.throttle) {
      body->applyForceAtLocalPoint(cp::Vect(300, 0), cp::Vect(0, 0));
    }
    auto angularVelocity = body->getAngularVelocity();
    if (a.direction == LEFT && angularVelocity >= -2) {
      body->setTorque(-10000);
    } else if (a.direction == RIGHT && angularVelocity <= 2) {
      body->setTorque(10000);
    } else {
      auto damping = -5000 * angularVelocity;
      body->setTorque(damping);
    }

    if (a.fire && !e.hardpoints.hardpoints.empty()) {
      sendMessage(e.hardpoints.hardpoints.front().weaponId, FireWeaponMessage());
    }
  }

  void onMessageReceived(const HitMessage& msg, MainFamily& e) {
    e.observer->reward += (.8 * (msg.physical + msg.energy)) / 100;
  }

  void onMessageReceived(const KillMessage& msg, MainFamily& e) {
    e.observer->reward += .8;
  }

  void onMessageReceived(const DamageMessage& msg, MainFamily& e) {
    e.observer->reward += -(msg.physical + msg.energy) / 100.f;
  }

  void onMessageReceived(const WipeMessage& msg, MainFamily& e) {
    limitTerminal = true;
  }

private:
  bool reachedGoal = false;
  bool limitTerminal = false;
  unsigned long long frames = 0;
};

REGISTER_SYSTEM(ControlSystem)
