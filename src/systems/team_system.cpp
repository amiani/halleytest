//
// Created by amiani on 5/2/20.
//

#include "systems/team_system.h"

class TeamSystem : public TeamSystemBase<TeamSystem> {
public:
  void update(Halley::Time t) {}

  std::unordered_map<int, std::vector<Halley::EntityId>> teams;
  void onEntitiesAdded(MainFamily& e) {
    teams[e.team.team].push_back(e.entityId);
  }

  void onEntitiesRemoved(MainFamily& e) {
    auto team = teams[e.team.team];
    team.erase(std::remove(team.begin(), team.end(), e.entityId), team.end());
    if (team.empty()) {
      sendMessage(teams[~e.team.team][0], WipeMessage());
    }
  }
};
REGISTER_SYSTEM(TeamSystem)