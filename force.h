#pragma once
#include "flecs.h"
#include "vector2d.h"

typedef Vector2D Force;

ECS_COMPONENT_EXTERN(Force);

#ifdef COMPONENT_IMPL
ECS_COMPONENT_DECLARE(Force);
#endif