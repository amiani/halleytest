#include "systems/control_system.h"
#include "chipmunk.hpp"
#include "src/game_stage.h"

class ControlSystem final : public ControlSystemBase<ControlSystem> {
public:
  void update(Halley::Time time, MainFamily& e) {
    if (terminal) {
      updateController(time, e, 1);
      getAPI().core->setStage(std::make_unique<GameStage>());
    } else {
      auto& action = updateController(time, e, -1);
      applyAction(e, action);
    }
  }

  const Action& updateController(Halley::Time time, MainFamily& e, int reward) {
    auto body = e.body.body;
    if (e.shipControl.controller->isObserver()) {
      return e.shipControl.controller->update(time, Observation(), reward);
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