#include "renderable.h"
#include <stdlib.h>

double _renderable_window_radius;
int _renderable_window_half_width;
int _renderable_window_half_height;

void renderable_init_window(int width, int height)
{
    _renderable_window_half_width = width / 2;
    _renderable_window_half_height = height / 2;

    _renderable_window_radius = sqrt(_renderable_window_half_width * _renderable_window_half_width +
                                     _renderable_window_half_height * _renderable_window_half_height);
}

Renderable renderable_new(Uint8 r, Uint8 g, Uint8 b, double x, double y)
{
    return (Renderable){(Vector2D){x, y}, 0, NULL, 0, r, g, b};
}

void renderable_push(Renderable *r, Vector2D a, Vector2D b)
{
    // allocate new memory
    Vector2D *new_points = realloc(r->points, 2 * (r->point_count + 1) * sizeof(Vector2D));
    if (new_points != NULL)
    {
        r->points = new_points;
        r->points[r->point_count * 2] = a;
        r->points[r->point_count * 2 + 1] = b;
        r->point_count += 1;
    }
}

void renderable_render_offset(Renderable *r, SDL_Renderer *rend, int x, int y)
{
    Vector2D center = (Vector2D){_renderable_window_half_width - x, _renderable_window_half_height - y};
    double radii_sum_squared = r->radius + _renderable_window_radius;
    radii_sum_squared *= radii_sum_squared;
    double distance_squared = vector_length_squared(vector_sub(center, r->origin));

    if (distance_squared <= radii_sum_squared)
    {
        SDL_SetRenderDrawColor(rend, r->r, r->g, r->b, SDL_ALPHA_OPAQUE);
        for (int i = 0; i < r->point_count; i++)
        {
            Vector2D *a = r->points + i * 2;
            Vector2D *b = a + 1;

            SDL_RenderDrawLine(rend, a->x + x, a->y + y, b->x + x, b->y + y);
        }
    }
}

void renderable_render(Renderable *r, SDL_Renderer *rend)
{
    SDL_SetRenderDrawColor(rend, r->r, r->g, r->b, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < r->point_count; i++)
    {
        Vector2D *a = r->points + i * 2;
        Vector2D *b = a + 1;

        SDL_RenderDrawLine(rend, a->x, a->y, b->x, b->y);
    }
}

void renderable_free(Renderable *r)
{
    free(r->points);
    r->points = NULL;
    r->point_count = 0;
}