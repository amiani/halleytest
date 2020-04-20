//
// Created by amiani on 4/20/20.
//

#pragma once

#include "actor.h"

class Trainer {
public:
  virtual Actor improve() =0;
};
