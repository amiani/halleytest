//
// Created by amiani on 4/20/20.
//

#include "sacactor.h"

Action SACActor::act(const Observation& o) {
  auto input = o.toTensor().to(DEVICE);
  auto out = module.forward({input}).toTensor();
  auto prob = (float)rand() / RAND_MAX;
  int i = 0;
  for (; i != 12, prob > 0; ++i) {
    prob -= out[i].item<float>();
  }
  --i;
  Action a{ .tensor=torch::full({1}, i) };
  if (i%3 == 0)       a.direction = LEFT;
  else if (i%3 == 1)  a.direction = RIGHT;
  else                a.direction = STRAIGHT;

  if (i < 6)  a.throttle = true;
  else        a.throttle = false;

  if ((i >= 3 && i <= 5) || i >= 9) a.fire = true;
  else                              a.fire = false;

  return a;
}