#include "dot.h"

Dot dot_from_vector(Vector2D v)
{
    return (Dot){v, v, (Vector2D){0, 0}, 0.97, 10, 1};
}

Dot dot_default()
{
    return dot_from_vector((Vector2D){0, 0});
}

static void render_vector_radius(const Vector2D v, SDL_Renderer *r, double radius)
{
    double subradius = (radius * 3) / 5;

    SDL_RenderDrawLine(r, v.x - subradius, v.y - subradius, v.x + subradius, v.y + subradius);
    SDL_RenderDrawLine(r, v.x - subradius, v.y + subradius, v.x + subradius, v.y - subradius);
}

void render_dot(const Dot d, SDL_Renderer *r, double x, double y)
{
    render_vector_radius(vector_add(d.position, (Vector2D){x, y}), r, d.radius);
}