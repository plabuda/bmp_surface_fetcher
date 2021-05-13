#include "stick.h"
#include "flecs.h"
#include "position.h"

Position *_stick_get_position(ecs_world_t *world, ecs_entity_t entity)
{
    ecs_ref_t ref;
    memset(&ref, 0, sizeof(ref));
    return (Position *)ecs_get_ref(world, &ref, entity, Position);
}

Position *stick_get_first(const Stick_Component *s, ecs_world_t *world)
{
    return _stick_get_position(world, s->first);
}

Position *stick_get_last(const Stick_Component *s, ecs_world_t *world)
{
    return _stick_get_position(world, s->last);
}

void _stick_constraint(ecs_iter_t *it)
{
    Stick_Component *s = ecs_column(it, Stick_Component, 1);
    for (int i = 0; i < it->count; i++)
    {
        Position *p_first = stick_get_first(s + i, it->world);
        Position *p_second = stick_get_last(s + i, it->world);

        Vector2D difference = vector_sub(*p_second, *p_first);
        Vector2D offset = vector_resize(difference, s[i].length / 2);
        Vector2D midpoint = vector_div(vector_add(*p_first, *p_second), 2);

        *p_first = vector_sub(midpoint, offset);
        *p_second = vector_add(midpoint, offset);
    }
}

ecs_entity_t stick_add_system(ecs_world_t *world, ecs_entity_t phase)
{
    ECS_COMPONENT_DEFINE(world, Stick_Component);
    ECS_SYSTEM(world, _stick_constraint, EcsOnUpdate, [in] Stick_Component);
    return _stick_constraint;
}