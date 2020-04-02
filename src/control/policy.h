#pragma once

#include "action.h"
#include "observation.h"

class Policy {
public:
  Action getAction(Observation& o);
};