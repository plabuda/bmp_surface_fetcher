#include "verlet.h"
#include "force.h"
#include "palette.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

void stick_update(Dot *first, Dot *second, double length, double stiffness)
{
    Vector2D difference = vector_sub(second->position, first->position);
    Vector2D offset = vector_resize(difference, length / 2);
    Vector2D midpoint = vector_div(vector_add(first->position, second->position), 2);

    first->position = vector_sub(midpoint, offset);
    second->position = vector_add(midpoint, offset);
}

void dot_update(Dot *d)
{
    Vector2D velocity = vector_sub(d->position, d->old_position);
    velocity = vector_mul(velocity, d->friction);

    d->old_position = d->position;
    d->position = vector_add(d->position, vector_add(velocity, d->gravity));
}

void update_dots(const Verlet *v)
{
    for (int i = 0; i < v->dot_count; i++)
    {
        dot_update(v->dots + i);
    }
}

void verlet_update(const Verlet *v)
{
    for (int i = 0; i < v->dot_count; i++)
    {
        dot_update(v->dots + i);
    }
}

void verlet_constrain(const Verlet *v)
{
    for (int i = 0; i < v->stick_count; i++)
    {
        Stick *s = v->sticks + i;
        Dot *first = v->dots + s->index_begin;
        Dot *second = v->dots + s->index_end;
        stick_update(first, second, s->length, s->stiffness);
    }
}

void verlet_step(const Verlet *v)
{
    static int counter = 0;

    counter++;
    counter %= 3;

    if (counter == 0)
    {
        verlet_update(v);
        for (int i = 0; i < 100; i++)
        {
            verlet_constrain(v);
        }
    }
}
void verlet_render_offset(const Verlet *v, SDL_Renderer *r, double x, double y)
{
    palette_set_color(Metal_Light);
    for (int i = 0; i < v->stick_count; i++)
    {
        Stick *s = v->sticks + i;

        Dot *first = v->dots + s->index_begin;
        Dot *second = v->dots + s->index_end;

        Vector2D first_pos = first->position;
        Vector2D second_pos = second->position;

        SDL_RenderDrawLine(r, first_pos.x + x, first_pos.y + y, second_pos.x + x, second_pos.y + y);
    }

    palette_set_color(Metal);
    for (int i = 0; i < v->dot_count; i++)
    {
        render_dot(v->dots[i], r, x, y);
    }
}

void verlet_render(const Verlet *v, SDL_Renderer *r)
{
    verlet_render_offset(v, r, 0, 0);
}

Verlet verlet_init(int dots_max, int sticks_max)
{
    return (Verlet){calloc(dots_max, sizeof(Dot)), dots_max, calloc(sticks_max, sizeof(Stick)), sticks_max};
}

void verlet_push_dots(Verlet *v, Vector2D *vectors, int vector_count)
{

    Dot *dots = realloc(v->dots, (v->dot_count + vector_count) * sizeof(Dot));
    if (dots != NULL)
    {
        v->dots = dots;
        for (int i = 0; i < vector_count; i++)
        {
            v->dots[v->dot_count + i] = dot_from_vector(vectors[i]);
        }
        v->dot_count += vector_count;
    }
}

void verlet_free(Verlet *v)
{
    free(v->dots);
    free(v->sticks);
    v->dot_count = v->stick_count = 0;
}

/////////////////////ECS/////////////////////////

void _verlet_move(ecs_iter_t *it)
{
    Position *p = ecs_column(it, Position, 1);
    OldPosition *Op = ecs_column(it, OldPosition, 2);

    for (int i = 0; i < it->count; i++)
    {

        Vector2D velocity = vector_sub(p[i], Op[i]);
        velocity = vector_mul(velocity, 0.97); // TODO Remove constant

        Op[i] = p[i];
        p[i] = vector_add(p[i], velocity);
    }
}

void _verlet_apply_forces(ecs_iter_t *it)
{
    OldPosition *Op = ecs_column(it, OldPosition, 1);
    Force *f = ecs_column(it, Force, 2);

    for (int i = 0; i < it->count; i++)
    {
        Op[i] = vector_sub(Op[i], f[i]);
    }
}

ecs_entity_t verlet_add_move_system(ecs_world_t *world, ecs_entity_t phase)
{
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, OldPosition);
    ECS_SYSTEM(world, _verlet_move, phase, Position, OldPosition);
    return _verlet_move;
}

ecs_entity_t verlet_add_force_system(ecs_world_t *world, ecs_entity_t phase)
{
    ECS_COMPONENT_DEFINE(world, OldPosition);
    ECS_COMPONENT_DEFINE(world, Force);
    ECS_SYSTEM(world, _verlet_apply_forces, phase, OldPosition, [in] Force);
    return _verlet_apply_forces;
}