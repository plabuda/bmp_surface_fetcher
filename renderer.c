#include "renderer.h"
#include "camera.h"
#include "globals.h"
#include "palette.h"
#include "position.h"
#include "renderable_utils.h"
#include "wheel.h"
#include <SDL2/SDL.h>

extern Globals G;

static const char *dot_query = "[in] Position, [in] OldPosition, [in] Renderable_Component";
static const char *line_query = "[in] Stick_Component, [in] Renderable_Component";
static const char *barrier_query = "[in] Position, [in] SolidBoxCollider, [in] Renderable_Component";
static const char *wheel_query = "[in] Wheel, [in] Position, [in] Renderable_Component";

typedef struct
{
    ecs_query_t *dot_query, //
        *line_query,        //
        *barrier_query,     //
        *wheel_query;       //
} RendererQueries;

typedef void (*RendererCallback)(ecs_iter_t *it, const Vector2D *camera);

static void run_renderer(const RendererCallback callback, const Vector2D *camera, ecs_query_t *query)
{
    ecs_iter_t it = ecs_query_iter(query);
    while (ecs_query_next(&it))
    {
        callback(&it, camera);
    }
}

static void draw_line(const Vector2D *first, const Vector2D *second)
{
    SDL_RenderDrawLine(G.renderer, first->x, first->y, second->x, second->y);
}

static void get_barrier_component_points(const Position *p, const SolidBoxCollider *bc, Vector2D *out)
{
    renderable_get_barrier_points(p->x, p->y,           //
                                  bc->angle, bc->width, //
                                  bc->height,           //
                                  out);                 //
}

static void render_wheels(ecs_iter_t *it, const Vector2D *camera)
{
    palette_set_color(Metal_Dark);
    Wheel *w = ecs_column(it, Wheel, 1);
    Position *p = ecs_column(it, Position, 2);

    for (int i = 0; i < it->count; i++)
    {
        Vector2D offset = vector_mul(w[i], 15);
        Vector2D pos = vector_sub(p[i], *camera);
        SDL_RenderDrawLine(G.renderer,                          //
                           pos.x + offset.x, pos.y + offset.y,  //
                           pos.x - offset.x, pos.y - offset.y); //
    }
}

static void render_barriers(ecs_iter_t *it, const Vector2D *camera)
{
    palette_set_color(Water_Light);

    Vector2D grid_points[4];
    Position *p = ecs_column(it, Position, 1);
    SolidBoxCollider *bc = ecs_column(it, SolidBoxCollider, 2);
    for (int i = 0; i < it->count; i++)
    {
        Vector2D origin = vector_sub(p[i], *camera);
        get_barrier_component_points(&origin, bc + i, grid_points);
        for (int j = 0; j < 4; j++)
        {
            int j1 = (j + 1) % 4;
            draw_line(grid_points + j, grid_points + j1);
        }

        draw_line(grid_points, grid_points + 2);
        draw_line(grid_points + 1, grid_points + 3);
    }
}

static void render_vector_radius(const Vector2D v, SDL_Renderer *r, double radius)
{
    double subradius = (radius * 3) / 5;

    SDL_RenderDrawLine(r, v.x - subradius, v.y - subradius, v.x + subradius, v.y + subradius);
    SDL_RenderDrawLine(r, v.x - subradius, v.y + subradius, v.x + subradius, v.y - subradius);
}

static void render_dots(ecs_iter_t *it, const Vector2D *camera)
{
    palette_set_color(Metal);
    Position *p = ecs_column(it, Position, 1);
    for (int i = 0; i < it->count; i++)
    {
        render_vector_radius(vector_sub(p[i], *camera), G.renderer, 10);
    }
}

static void render_lines(ecs_iter_t *it, const Vector2D *camera)
{
    Vector2D first;
    Vector2D second;
    palette_set_color(Water_Light);

    Stick_Component *s = ecs_column(it, Stick_Component, 1);
    for (int i = 0; i < it->count; i++)
    {
        const Position *p_first = ecs_get(it->world, s[i].first, Position);
        first = vector_sub(*p_first, *camera);
        const Position *p_second = ecs_get(it->world, s[i].last, Position);
        second = vector_sub(*p_second, *camera);
        draw_line(&first, &second);
    }
}

static void renderer_render(ecs_iter_t *it)
{
    const RendererQueries *qr = it->param;
    const Camera *c = ecs_column(it, Camera, 1);
    const Position *p = ecs_column(it, Position, 2);
    //  printf("Camera system called for %u entities\n", it->count);
    for (int i = 0; i < it->count; i++)
    {
        SDL_SetRenderTarget(G.renderer, NULL);
        const Vector2D *offset = p + i;
        run_renderer(render_dots, offset, qr->dot_query);
        run_renderer(render_lines, offset, qr->line_query);
        run_renderer(render_wheels, offset, qr->wheel_query);
        SDL_SetRenderTarget(G.renderer, c[i].background);
        SDL_SetRenderDrawColor(G.renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(G.renderer);
        run_renderer(render_barriers, offset, qr->barrier_query);
        //  printf("Camera %d is at position %f, %f\n", i, p[i].x, p[i].y);
    }

    SDL_SetRenderTarget(G.renderer, NULL);
}

ecs_entity_t renderer_add_rendering_system(ecs_world_t *world, ecs_entity_t phase)
{
    printf("%s\n", dot_query);
    RendererQueries *rq = malloc(sizeof(RendererQueries));
    rq->dot_query = ecs_query_new(world, dot_query);
    rq->line_query = ecs_query_new(world, line_query);
    rq->barrier_query = ecs_query_new(world, barrier_query);
    rq->wheel_query = ecs_query_new(world, wheel_query);
    ECS_COMPONENT_DEFINE(world, Camera);
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_SYSTEM(world, renderer_render, phase, [in] Camera, [in] Position);
    ecs_set(world, renderer_render, EcsContext, {rq});
    return renderer_render;
}