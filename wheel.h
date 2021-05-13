#pragma once
#include "flecs.h"
#include "vector2d.h"

typedef Vector2D Wheel;

ECS_COMPONENT_EXTERN(Wheel);

#ifdef COMPONENT_IMPL
ECS_COMPONENT_DECLARE(Wheel);
#endif

ecs_entity_t wheel_add_correction_system(ecs_world_t *world,
                                         ecs_entity_t phase);

float wheel_accelerate(Vector2D *old_position, const Vector2D *position,   //
                       const Vector2D *forward_vector, float acceleration, //
                       float limit_upper, float limit_lower);              //

ecs_entity_t wheel_add_acceleration_system(ecs_world_t *world,
                                           ecs_entity_t phase);