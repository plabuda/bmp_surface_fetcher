#pragma once
#include "flecs.h"

typedef struct {
  float limit_upper;
  float limit_lower;
  float acceleration;
} Acceleration;

ECS_COMPONENT_EXTERN(Acceleration);

#ifdef COMPONENT_IMPL
ECS_COMPONENT_DECLARE(Acceleration);
#endif
