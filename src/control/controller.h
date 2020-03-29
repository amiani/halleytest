#pragma once

#include <halley.hpp>
#include "action.h"
#include "src/control/observation.h"

class Controller {
public:
  virtual const Action& update(Halley::Time t, const cp::Vect& p, Observation& o) = 0;
  virtual const Action& update(Time t, const cp::Vect& p) = 0;
  const Action& getAction();

protected:
  std::vector<std::shared_ptr<Action>> actions;
  std::vector<std::shared_ptr<Observation>> observations;
  std::vector<int> rewards;
};

class InputController : public Controller {
public:
  InputController(InputVirtual& device);
  const Action& update(Time t, const cp::Vect& p, Observation& o) override;
  const Action& update(Time t, const cp::Vect& p) override;

private:
  InputVirtual device;
};

class RLController : public Controller {
public:
  const Action& update(Halley::Time t, const cp::Vect& p, Observation& o) override;
};