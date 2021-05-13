#include "checkpoint.h"
#include "bike.h"
#include "box_collider.h"
#include "globals.h"
#include "palette.h"
#include "position.h"
#include "renderable_utils.h"
#include "stick.h"
#include <SDL2/SDL.h>

extern Globals G;

void _draw_line(const Vector2D *first, const Vector2D *second)
{
    SDL_RenderDrawLine(G.renderer, first->x, first->y, second->x, second->y);
}

typedef struct
{
    CheckpointTimes *times;
    ecs_query_t *bike_query;
} _checkpoint_context;

typedef struct
{
    const CheckpointTimes *times;
    const Position *camera;
} _checkpoint_render_context;

CheckpointTimes checkpoint_request_new(ecs_world_t *world, Uint32 checkpoint_count)
{
    CheckpointTimes result = {0, 0, checkpoint_count, 0};
    result.first_checkpoint = *ecs_bulk_new(world, 0, checkpoint_count);
    return result;
}

int _checkpoint_order_compare(ecs_entity_t e1, const void *c1, ecs_entity_t e2, const void *c2)
{
    return e1 - e2;
}

void _checkpoint_render(const Position *p, const BoxColliderComponent *bc, int cells, Vector2D *grid_points)
{
    renderable_get_checkpoint_points(p->x, p->y, bc->angle, cells, grid_points);
    for (int i = 0; i < cells; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if ((i + j) % 2 == 0)
            {
                _draw_line(&grid_points[i * 3 + j], &grid_points[(i + 1) * 3 + j]);
                _draw_line(&grid_points[i * 3 + j], &grid_points[i * 3 + (j + 1)]);
                _draw_line(&grid_points[(i + 1) * 3 + j], &grid_points[(i + 1) * 3 + (j + 1)]);
                _draw_line(&grid_points[i * 3 + (j + 1)], &grid_points[(i + 1) * 3 + (j + 1)]);

                _draw_line(&grid_points[i * 3 + j], &grid_points[(i + 1) * 3 + (j + 1)]);
                _draw_line(&grid_points[(i + 1) * 3 + j], &grid_points[i * 3 + (j + 1)]);
            }
            else
            {
            }
        }
    }

    _draw_line(&grid_points[0], &grid_points[cells * 3]);
    _draw_line(&grid_points[0], &grid_points[2]);
    _draw_line(&grid_points[cells * 3], &grid_points[cells * 3 + 2]);
    _draw_line(&grid_points[2], &grid_points[cells * 3 + 2]);
}

void _checkpoint_render_checkpoints(ecs_iter_t *it)
{
    const _checkpoint_render_context *crc = it->param;
    const CheckpointTimes *ct = crc->times;
    const Position *camera = crc->camera;

    const int cells = 8;
    Vector2D grid_points[(cells + 1) * 3];

    Position *p = ecs_column(it, Position, 1);
    BoxColliderComponent *bc = ecs_column(it, BoxColliderComponent, 2);
    Checkpoint *c = ecs_column(it, Checkpoint, 3);

    const Sint8 offset = checkpoint_find_offset(ct, it);
    if (offset < 0)
    {
        palette_set_color(Rust_Dark);
        for (int i = 0; i < it->count; i++)
        {
            Position origin = vector_sub(p[i], *camera);
            _checkpoint_render(&origin, bc + i, cells, grid_points);
        }
    }
    else
    {
        int i = 0;
        palette_set_color(Rust_Dark);
        while (i < offset && i < it->count)
        {
            Position origin = vector_sub(p[i], *camera);
            _checkpoint_render(&origin, bc + i, cells, grid_points);
            i++;
        }
        palette_set_color(ct->current_checkpoint == 0 || ct->current_checkpoint == ct->checkpoint_count ? Water_Light
                                                                                                        : Water_Dark);
        if (i < it->count)
        {
            Position origin = vector_sub(p[i], *camera);
            _checkpoint_render(&origin, bc + i, cells, grid_points);
            i++;
        }
        palette_set_color(ct->current_checkpoint == ct->checkpoint_count ? Rust_Dark : Sand_Dark);
        while (i < it->count)
        {
            Position origin = vector_sub(p[i], *camera);
            _checkpoint_render(&origin, bc + i, cells, grid_points);
            i++;
        }
    }
}

void _checkpoint_check_collision(ecs_iter_t *it)
{
    const _checkpoint_context *ctx = it->param;

    Position *p = ecs_column(it, Position, 1);
    BoxColliderComponent *bc = ecs_column(it, BoxColliderComponent, 2);
    Checkpoint *c = ecs_column(it, Checkpoint, 3);

    Sint8 offset = checkpoint_find_offset(ctx->times, it);
    if (offset >= 0)
    {
        const BoxColliderComponent *checkpoint_bc = bc + offset;
        const Position *checkpoint_p = p + offset;
        Checkpoint *checkpoint_c = c + offset;

        if (checkpoint_c->timestamp == 0 || ctx->times->current_checkpoint == ctx->times->checkpoint_count)
        {
            ecs_iter_t bike_iter = ecs_query_iter(ctx->bike_query);
            while (ecs_query_next(&bike_iter))
            {

                const Stick_Component *sc = ecs_column(&bike_iter, Stick_Component, 2);
                for (int j = 0; j < bike_iter.count; j++)
                {
                    const Position *first = ecs_get(bike_iter.world, (sc + j)->first, Position);
                    const Position *last = ecs_get(bike_iter.world, (sc + j)->last, Position);

                    if (collider_check_vector(*checkpoint_p, checkpoint_bc->angle,            //
                                              checkpoint_bc->width, checkpoint_bc->height,    //
                                              *first, 5)                                      //
                        || collider_check_vector(*checkpoint_p, checkpoint_bc->angle,         //
                                                 checkpoint_bc->width, checkpoint_bc->height, //
                                                 *last, 5))                                   //
                    {
                        if (checkpoint_c->timestamp == 0)
                        {
                            checkpoint_c->timestamp = SDL_GetTicks();
                            printf("Bike component %llu collided with checkpoint "
                                   "%llu at time %u\n",
                                   bike_iter.entities[j], it->entities[offset], checkpoint_c->timestamp);
                        }
                        else
                        {
                            ctx->times->final_timestamp = SDL_GetTicks();

                            printf("Bike component %llu collided with final checkpoint "
                                   "%llu at time %u\n",
                                   bike_iter.entities[j], it->entities[offset], ctx->times->final_timestamp);
                        }
                        ctx->times->current_checkpoint++;
                    }
                }
            }
        }
    }
    // printf("checkpoint check called for %d checkpoints\n", it->count);
}

Sint8 checkpoint_find_offset(const CheckpointTimes *ct, ecs_iter_t *it)
{
    ecs_entity_t target = ct->first_checkpoint + (ct->current_checkpoint % ct->checkpoint_count);
    if (it->entities[0] > target || it->entities[it->count - 1] < target ||
        ct->current_checkpoint > ct->checkpoint_count)
    {
        return -1;
    }
    else
    {
        return target - it->entities[0];
    }
}

void checkpoint_set(ecs_world_t *world, ecs_entity_t entity, float x, float y, float angle)
{
    ecs_add(world, entity, Renderable_Component);
    ecs_set(world, entity, BoxColliderComponent, {200, 50, angle});
    ecs_set(world, entity, Position, {x, y});
    ecs_add(world, entity, Checkpoint);
}

ecs_entity_t checkpoint_add_collision_system(ecs_world_t *world, ecs_entity_t phase, CheckpointTimes *context)
{
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Checkpoint);
    ECS_COMPONENT_DEFINE(world, BoxColliderComponent);
    ECS_COMPONENT_DEFINE(world, Stick_Component);
    ECS_COMPONENT_DEFINE(world, Bike_Component);

    ECS_SYSTEM(world, _checkpoint_check_collision, phase, [in] Position, [in] BoxColliderComponent, Checkpoint);
    const EcsQuery *q = ecs_get(world, _checkpoint_check_collision, EcsQuery);
    ecs_query_order_by(world, q->query, 0, _checkpoint_order_compare);

    ecs_query_t *bike_query = ecs_query_new(world, "[in] Bike_Component, [in] Stick_Component");

    _checkpoint_context *ctx = malloc(sizeof(_checkpoint_context));
    ctx->times = context;
    ctx->bike_query = bike_query;

    ecs_set(world, _checkpoint_check_collision, EcsContext, {ctx});

    return _checkpoint_check_collision;
}
ecs_entity_t checkpoint_add_rendering_system(ecs_world_t *world, ecs_entity_t phase, CheckpointTimes *context,
                                             Position *camera)
{
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Checkpoint);
    ECS_COMPONENT_DEFINE(world, BoxColliderComponent);
    ECS_COMPONENT_DEFINE(world, Renderable_Component);
    ECS_SYSTEM(world, _checkpoint_render_checkpoints,
               phase, [in] Position, [in] BoxColliderComponent, [in] Checkpoint, [in] Renderable_Component);
    const EcsQuery *q = ecs_get(world, _checkpoint_render_checkpoints, EcsQuery);
    ecs_query_order_by(world, q->query, 0, _checkpoint_order_compare);

    _checkpoint_render_context *crc = malloc(sizeof(_checkpoint_render_context));
    *crc = (_checkpoint_render_context){context, camera};
    ecs_set(world, _checkpoint_render_checkpoints, EcsContext, {crc});

    return _checkpoint_render_checkpoints;
}