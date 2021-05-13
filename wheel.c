#include "wheel.h"
#include "acceleration.h"
#include "flecs.h"
#include "force.h"
#include "position.h"

void _wheel_correct(ecs_iter_t *it)
{
    OldPosition *Op = ecs_column(it, OldPosition, 1);
    Position *p = ecs_column(it, Position, 2);
    Wheel *w = ecs_column(it, Wheel, 3);
    ecs_entity_t *e = it->entities;

    for (int i = 0; i < it->count; i++)
    {
        // Calculate the velocity that the dot will take
        Vector2D velocity = vector_sub(p[i], Op[i]);

        // Calculate the "valid" part of the velocity
        Vector2D correct_velocity = vector_cast(velocity, w[i]);

        Vector2D correction = vector_sub(correct_velocity, velocity);
        ecs_set(it->world, e[i], Force, {correction.x, correction.y});
    }
}

ecs_entity_t wheel_add_correction_system(ecs_world_t *world, ecs_entity_t phase)
{
    ECS_COMPONENT_DEFINE(world, OldPosition);
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Force);
    ECS_COMPONENT_DEFINE(world, Wheel);
    ECS_SYSTEM(world, _wheel_correct, phase, [in] OldPosition, [in] Position, [in] Wheel);
    return _wheel_correct;
}

float wheel_accelerate(Vector2D *old_position, const Vector2D *position,   //
                       const Vector2D *forward_vector, float acceleration, //
                       float limit_upper, float limit_lower)               //
{
    const Vector2D velocity = vector_sub(*position, *old_position);

    float speed = vector_dot(velocity, *forward_vector);
    Vector2D velocity_along = vector_mul(*forward_vector, speed);
    Vector2D velocity_perp = vector_sub(velocity, velocity_along);

    speed += acceleration;
    speed = speed >= limit_upper ? limit_upper :              //
                (speed <= limit_lower ? limit_lower : speed); //

    Vector2D new_velocity_along = vector_mul(*forward_vector, speed);
    Vector2D new_velocity = vector_add(new_velocity_along, velocity_perp);
    *old_position = vector_sub(*position, new_velocity);

    return speed;
}

float _wheel_accelerate(OldPosition *Op, const Position *p, const Wheel *w, const Acceleration *a)
{
    return wheel_accelerate(Op, p, w, a->acceleration, a->limit_upper, a->limit_lower);
}

void _wheel_acceleration_system(ecs_iter_t *it)
{
    OldPosition *Op = ecs_column(it, OldPosition, 1);
    Position *p = ecs_column(it, Position, 2);
    Wheel *w = ecs_column(it, Wheel, 3);
    Acceleration *a = ecs_column(it, Acceleration, 4);
    for (int i = 0; i < it->count; i++)
    {
        _wheel_accelerate(Op + i, p + i, w + i, a + i);
    }
}

ecs_entity_t wheel_add_acceleration_system(ecs_world_t *world, ecs_entity_t phase)
{
    ECS_COMPONENT_DEFINE(world, OldPosition);
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Wheel);
    ECS_COMPONENT_DEFINE(world, Acceleration);
    ECS_SYSTEM(world, _wheel_acceleration_system, phase, OldPosition, [in] Position, [in] Wheel, [in] Acceleration);
    return _wheel_acceleration_system;
}