//
// Created by amiani on 5/1/20.
//

#include "systems/detection_system.h"

class DetectionSystem final : public DetectionSystemBase<DetectionSystem> {
public:
  void init() {

  }

  void update(Halley::Time t) {}

  void onMessageReceived(const DetectMessage& msg, MainFamily& e) {
    auto& entities = e.detector.entities;
    if (msg.detected) {
      entities.push_back(msg.id);
    } else {
      entities.erase(std::remove(entities.begin(), entities.end(), msg.id), entities.end());
    }
  }
};

REGISTER_SYSTEM(DetectionSystem);