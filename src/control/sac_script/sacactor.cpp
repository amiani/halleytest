//
// Created by amiani on 4/20/20.
//

#include "sacactor.h"

Action SACActor::act(const Observation& o) {
  auto input = o.toTensor().unsqueeze(0).to(DEVICE);
  auto out = module.forward({input}).toTuple()->elements();
  int i = out[0].toTensor().item<int>();

  Action a{ .tensor=out[0].toTensor() };
  if (i%3 == 0)       a.direction = LEFT;
  else if (i%3 == 1)  a.direction = RIGHT;
  else                a.direction = STRAIGHT;

  /*
  if (i < 6)  a.throttle = true;
  else        a.throttle = false;

  if ((i >= 3 && i <= 5) || i >= 9) a.fire = true;
  else                              a.fire = false;
   */
  a.throttle = true;
  a.fire = false;

  return a;
}
