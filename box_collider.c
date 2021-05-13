#include "box_collider.h"
#include "position.h"
#include "vector2d.h"
#include <SDL2/SDL.h>

Vector2D _box_get_unrotated_vector(const Vector2D rotated, const Vector2D center, const double angle)
{
    return vector_rotate(vector_sub(rotated, center), -1 * angle);
}

Vector2D _box_get_rotated_vector(const Vector2D unrotated, const Vector2D center, const double angle)
{
    return vector_add(center, vector_rotate(unrotated, angle));
}

Uint8 _box_check_axis_collision(double value, double radius, double limit)
{
    return (value < radius + limit / 2 && value > -radius - limit / 2);
}

Uint8 _box_check_rotated_vector(const Vector2D diff, double radius, double width, double height)
{
    return _box_check_axis_collision(diff.x, radius, width) && _box_check_axis_collision(diff.y, radius, height);
}

Uint8 collider_check_vector(Vector2D center, double angle, double width, double height, Vector2D v, double radius)
{
    Vector2D diff = _box_get_unrotated_vector(v, center, angle);
    return _box_check_rotated_vector(diff, radius, width, height);
}

Uint8 collider_check(BoxCollider *bc, Dot *d)
{
    return collider_check_vector(bc->center, bc->angle, bc->width, bc->height, d->position, d->radius);
}

double _box_collide_with_axis(double in, double radius, double margin, double limit)
{
    return in > 0 ? radius + limit / 2 + margin : -radius - limit / 2 - margin;
}

Uint8 collider_collide_vector(Vector2D center, double angle, double width, double height, Vector2D *v, double radius)
{
    double margin = 3;

    Vector2D diff = _box_get_unrotated_vector(*v, center, angle);

    if (_box_check_rotated_vector(diff, radius, width, height))
    {
        if (width < height)
        {
            diff.x = _box_collide_with_axis(diff.x, radius, margin, width);
        }
        else
        {
            diff.y = _box_collide_with_axis(diff.y, radius, margin, height);
        }

        *v = _box_get_rotated_vector(diff, center, angle);
        return 1;
    }

    return 0;
}

Uint8 collider_collide(BoxCollider *bc, Dot *d)
{
    return collider_collide_vector(bc->center, bc->angle, bc->width, bc->height, &d->position, d->radius);
}

Uint8 collider_component_collide(const SolidBoxCollider *bc, Vector2D center, Vector2D *position, double radius)
{
    return collider_collide_vector(center, bc->angle, bc->width, bc->height, position, radius);
}

/////////////////////ECS////////////////////

void _collider_system(ecs_iter_t *it)
{
    ecs_query_t *collider_query = it->param;
    ecs_iter_t collider_iter = ecs_query_iter(collider_query);

    Position *p = ecs_column(it, Position, 1);

    int count = 0;

    Uint8 crash = SDL_FALSE;

    while (ecs_query_next(&collider_iter))
    {
        const Position *collider_p = ecs_column(&collider_iter, Position, 1);
        const SolidBoxCollider *collider_bc = ecs_column(&collider_iter, SolidBoxCollider, 2);
        for (int j = 0; j < collider_iter.count; j++)
        {
            for (int i = 0; i < it->count; i++)
            {
                crash = collider_component_collide(collider_bc + j, collider_p[j], p + i, 5) || crash;
            }
        }
    }

    if (crash)
    {
        // printf("Crash! %u\n", SDL_GetTicks());
    }
}

ecs_entity_t collider_add_collision_system(ecs_world_t *world, ecs_entity_t phase)
{
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, OldPosition);
    ECS_COMPONENT_DEFINE(world, SolidBoxCollider);

    ECS_SYSTEM(world, _collider_system, phase, Position, OldPosition);

    ecs_query_t *collider_query = ecs_query_new(world, "[in] Position, [in] SolidBoxCollider");

    ecs_set(world, _collider_system, EcsContext, {collider_query});
    return _collider_system;
}