#pragma once
#include "flecs.h"
#include "vector2d.h"

typedef Vector2D Position;
typedef Position OldPosition;

ECS_COMPONENT_EXTERN(Position);
ECS_COMPONENT_EXTERN(OldPosition);

#ifdef COMPONENT_IMPL
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(OldPosition);
#endif