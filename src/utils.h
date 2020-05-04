#pragma once

#include "chipmunk.hpp"
#include <halley.hpp>
#include "torch/torch.h"

Halley::Vector2f chipToScreen(cp::Vect, cp::Vect);
cp::Vect screenToChip(Halley::Vector2f, cp::Vect);
Halley::Vector2f chipToHalley(const cp::Vect&);
cp::Vect halleyToChip(const Halley::Vector2f);

const torch::Device DEVICE(torch::kCUDA);
bool isAnyNAN(const torch::Tensor&);
bool isAnyZero(const torch::Tensor&);

enum BodyType {
  SHIPBODY,
  PROJECTILEBODY,
  DETECTORBODY,
  ASTEROIDBODY,
  GOALBODY
};

enum CollisionFilter {
  fTEAM0SHIP = (1u << 0),
  fTEAM1SHIP = (1u << 1),
  fDETECTOR = (1u << 2),
  fPROJECTILE = (1u << 3),
  fGOAL = (1u << 4),
  fASTEROID = (1u << 5),
  fBOUNDARY = (1u << 6),
};
