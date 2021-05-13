#include "world.h"
#include "bike.h"
#include "box_collider.h"
#include "dot.h"
#include "flecs.h"
#include "palette.h"
#include "renderable.h"
#include "renderable_utils.h"
#include "verlet.h"
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// typedef struct
// {
//     float x;
//     float y;
// } Position, Velocity;

// void Move(ecs_iter_t *it)
// {
//     Position *p = ecs_column(it, Position, 1);
//     Velocity *v = ecs_column(it, Velocity, 2);

//     for (int i = 0; i < it->count; i++)
//     {
//         p[i].x += v[i].x * it->delta_time;
//         p[i].y += v[i].y * it->delta_time;
//         printf("Entity %s moved! {%f, %f}\n", ecs_get_name(it->world, it->entities[i]), p[i].x, p[i].y);
//     }
// }

World *world_new(double x, double y)
{
    World *result = calloc(1, sizeof(World));

    // flecs world init
    // ecs_world_t *ecs = ecs_init();
    // result->ecs_world = ecs;

    // ECS_COMPONENT(ecs, Position);
    // ECS_COMPONENT(ecs, Velocity);

    // ECS_SYSTEM(ecs, Move, EcsOnUpdate, Position, [in] Velocity);

    // ecs_entity_t e = ecs_set(ecs, 0, EcsName, {"MyEntity"});
    // ecs_set(ecs, e, Position, {0, 0});
    // ecs_set(ecs, e, Velocity, {1, 1});
    //

    Verlet *v = malloc(sizeof(Verlet));
    *v = verlet_init(2, 1);

    v->dots[0] = dot_from_vector((Vector2D){x + 25, y});
    v->dots[1] = dot_from_vector((Vector2D){x - 25, y});

    v->sticks[0] = (Stick){0, 1, 50, 2};

    result->b.back_index = 0;
    result->b.back_index = 1;
    result->b.v = v;

    return result;
}

void _checkpoint_start(Renderable *r)
{
    r->r = palette_get_r(Water_Light);
    r->g = palette_get_g(Water_Light);
    r->b = palette_get_b(Water_Light);
}

void _checkpoint_disabled(Renderable *r)
{
    r->r = palette_get_r(Sand_Dark);
    r->g = palette_get_g(Sand_Dark);
    r->b = palette_get_b(Sand_Dark);
}

void _checkpoint_enabled(Renderable *r)
{
    r->r = palette_get_r(Water);
    r->g = palette_get_g(Water);
    r->b = palette_get_b(Water);
}

void _checkpoint_cleared(Renderable *r)
{
    r->r = palette_get_r(Rust_Dark);
    r->g = palette_get_g(Rust_Dark);
    r->b = palette_get_b(Rust_Dark);
}

void render_background(SDL_Renderer *r, double x, double y)
{
    int x_int = x / 200;
    x -= (x_int)*200;

    if (x < 0)
    {
        x += 200;
    }

    int y_int = y / 200;
    y -= (y_int)*200;

    if (y < 0)
    {
        y += 200;
    }

    palette_set_color(Sand_Dark);

    for (int i = 0; i < 4; i++)
    {
        SDL_RenderDrawLine(r, x + i * 200, 0, x + i * 200, 600);
    }

    for (int i = 0; i < 3; i++)
    {
        SDL_RenderDrawLine(r, 0, 200 * i + y, 800, 200 * i + y);
    }
}

Vector2D world_get_camera(World *w)
{
    return vector_add(vector_mul(vector_add(w->b.v->dots[0].position, w->b.v->dots[1].position), -0.5),
                      (Vector2D){400, 300});
}

void world_steer(World *w, int turn, int speed)
{
    bike_steer(&(w->b), turn, speed);
}

Uint8 world_test_checkpoint(World *w)
{
    if (w->checkpoint_count > 0)
    {
        // Normal case, including no checkpoint hit
        if (w->current_checkpoint < w->checkpoint_count)
        {
            // Collide against current checkpoint
            BoxCollider *bc = w->checkpoint_colliders + w->current_checkpoint;
            if (collider_check(bc, w->b.v->dots) || collider_check(bc, w->b.v->dots + 1))
            {
                _checkpoint_cleared(w->checkpoint_renderables + w->current_checkpoint);
                w->checkpoint_times[w->current_checkpoint] = SDL_GetTicks();
                printf("Checkpoint %d reached at time %u", w->current_checkpoint,
                       w->checkpoint_times[w->current_checkpoint]);

                if (w->current_checkpoint > 0)
                {
                    printf(" Checkpoint time: %u\n",
                           w->checkpoint_times[w->current_checkpoint] - w->checkpoint_times[w->current_checkpoint - 1]);
                }
                else
                {
                    printf("\n");
                }

                w->current_checkpoint++;
                if (w->current_checkpoint < w->checkpoint_count)
                {
                    _checkpoint_enabled(w->checkpoint_renderables + w->current_checkpoint);
                }
                else
                {
                    _checkpoint_start(w->checkpoint_renderables);
                }

                return SDL_TRUE;
            }
        }
        // Final case - return to start line
        else if (w->current_checkpoint == w->checkpoint_count)
        {
            BoxCollider *bc = w->checkpoint_colliders;
            if (collider_check(bc, w->b.v->dots) || collider_check(bc, w->b.v->dots + 1))
            {
                _checkpoint_cleared(w->checkpoint_renderables);
                w->checkpoint_times[w->checkpoint_count] = SDL_GetTicks();
                printf("Finish line reached at time %u\n", w->checkpoint_times[w->checkpoint_count]);

                w->current_checkpoint++;

                printf("Total time: %u\n", w->checkpoint_times[w->checkpoint_count] - w->checkpoint_times[0]);
                return SDL_TRUE;
            }
        }
    }
    return SDL_FALSE;
}

void world_update(World *w, Uint8 *play_beep, Uint8 *play_crash)
{

    Bike *b = &(w->b);
    Verlet *v = (w->b.v);
    bike_update(b);
    verlet_update(v);

    Uint8 crash = SDL_FALSE;

    for (int j = 0; j < w->wall_count; j++)
    {
        BoxCollider *bc = w->wall_colliders + j;
        crash = collider_collide(bc, w->b.v->dots) || crash;
        crash = collider_collide(bc, w->b.v->dots + 1) || crash;
    }

    Uint8 beep = world_test_checkpoint(w);

    if (play_beep != NULL)
    {
        *play_beep = beep;
    }

    if (play_crash != NULL)
    {
        *play_crash = crash;
    }

    for (int i = 0; i < 50; i++)
    {
        verlet_constrain(v);
        bike_constrain(b);
    }

    // ecs_progress(w->ecs_world, 0);
}

void world_render(SDL_Renderer *r, World *w)
{
    Vector2D v = world_get_camera(w);
    world_render_offset(r, w, v.x, v.y);
}

void world_render_offset(SDL_Renderer *r, World *w, float x, float y)
{
    render_background(r, x, y);

    bike_render_offset(&(w->b), r, x, y);

    for (int i = 0; i < w->checkpoint_count; i++)
    {
        renderable_render_offset(w->checkpoint_renderables + i, r, x, y);
    }

    for (int i = 0; i < w->wall_count; i++)
    {
        renderable_render_offset(w->wall_renderables + i, r, x, y);
    }

    verlet_render_offset((w->b.v), r, x, y);
}

int _world_resize_memory(Renderable **r, BoxCollider **b, int size)
{
    Renderable *r_new = realloc(*r, size * sizeof(Renderable));
    BoxCollider *b_new = realloc(*b, size * sizeof(BoxCollider));

    if (size == 0 || (b_new != NULL && r_new != NULL))
    {
        *b = b_new;
        *r = r_new;
        return 1;
    }
    else
    {
        return 0;
    }
}

int _world_resize_checkpoints(World *w, int size)
{
    if (_world_resize_memory(&(w->checkpoint_renderables), &(w->checkpoint_colliders), size))
    {
        w->checkpoint_count = size;
        Uint32 *times = realloc(w->checkpoint_times, (size + 1) * sizeof(Uint32));
        if (size == 0 || times != NULL)
        {
            w->checkpoint_times = times;
        }
        return 1;
    }

    return 0;
}

int _world_resize_walls(World *w, int size)
{
    if (_world_resize_memory(&(w->wall_renderables), &(w->wall_colliders), size))
    {
        w->wall_count = size;
        return 1;
    }
    return 0;
}

void world_add_checkpoint(World *w, double x, double y, double angle)
{
    if (_world_resize_checkpoints(w, w->checkpoint_count + 1))
    {
        w->checkpoint_renderables[w->checkpoint_count - 1] = renderable_get_checkpoint(x, y, angle);
        w->checkpoint_colliders[w->checkpoint_count - 1] = (BoxCollider){(Vector2D){x, y}, 200, 50, angle};
    }
}

void world_add_wall(World *w, double x, double y, double angle, double width, double height)
{

    if (_world_resize_walls(w, w->wall_count + 1))
    {
        w->wall_renderables[w->wall_count - 1] = renderable_get_barrier(x, y, angle, width, height);
        w->wall_colliders[w->wall_count - 1] = (BoxCollider){(Vector2D){x, y}, width, height, angle};
    }
}

void world_reset_times(World *w)
{
    for (int i = 0; i < w->checkpoint_count; i++)
    {
        w->checkpoint_times[i] = 0;
        _checkpoint_disabled(w->checkpoint_renderables + i);
    }

    w->checkpoint_times[w->checkpoint_count] = 0;
    _checkpoint_start(w->checkpoint_renderables);
}

void world_free(World **wp)
{
    World *w = *wp;
    if (w != NULL)
    {
        // ecs_fini(w->ecs_world);
        for (int i = 0; i < w->wall_count; i++)
        {
            renderable_free(w->wall_renderables + i);
        }

        free(w->wall_renderables);
        w->wall_renderables = NULL;
        free(w->wall_colliders);
        w->wall_colliders = NULL;
        w->wall_count = 0;

        for (int i = 0; i < w->checkpoint_count; i++)
        {
            renderable_free(w->checkpoint_renderables + i);
        }

        free(w->checkpoint_renderables);
        w->checkpoint_renderables = NULL;

        free(w->checkpoint_colliders);
        w->checkpoint_colliders = NULL;

        w->checkpoint_count = 0;

        verlet_free(w->b.v);
        free(w->b.v);
        free(w);
        *wp = NULL;
    }
}

World *world_from_floats(float *stream)
{
    World *result = world_new(100, 100);
    Vector2D current = (Vector2D){300, 0};
    Vector2D offset = (Vector2D){380, 0};

    int wall_len = stream[0];
    int stream_offset = 1;

    for (int i = 0; i < wall_len; i++)
    {
        world_add_wall(result,                    //
                       stream[stream_offset],     //
                       stream[stream_offset + 1], //
                       stream[stream_offset + 2], //
                       stream[stream_offset + 3], //
                       stream[stream_offset + 4]);
        stream_offset += 5;
    }

    int checkpoint_len = stream[stream_offset];
    stream_offset++;

    for (int i = 0; i < checkpoint_len; i++)
    {

        world_add_checkpoint(result,                    //
                             stream[stream_offset],     //
                             stream[stream_offset + 1], //
                             stream[stream_offset + 2]);
        stream_offset += 3;
    }

    world_reset_times(result);
    return result;
}