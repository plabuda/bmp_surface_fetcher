#include "renderable_utils.h"
#include "palette.h"
#include "vector2d.h"

double renderable_get_checkpoint_points(double x, double y, double angle, int cells, Vector2D *grid_points)
{
    const double checkpoint_width = 200;

    const double hw = checkpoint_width / 2;
    const double cell_height = checkpoint_width / cells;

    for (int i = 0; i <= cells; i++)
    {
        int shifted_index = i - cells / 2;
        double w = (checkpoint_width * shifted_index) / cells;
        for (int j = 0; j < 3; j++)
        {
            double h = (j - 1) * cell_height;
            grid_points[i * 3 + j] = (Vector2D){w, h};
        }
    }

    for (int i = 0; i < cells + 1; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            grid_points[i * 3 + j] = vector_add(vector_rotate(grid_points[i * 3 + j], angle), (Vector2D){x, y});
        }
    }

    return sqrt(hw * hw + cell_height * cell_height);
}

Renderable renderable_get_checkpoint(double x, double y, double angle)
{
    Renderable result = renderable_new(255, 0, 0, x, y);

    const int cells = 8;
    Vector2D grid_points[(cells + 1) * 3];

    result.radius = renderable_get_checkpoint_points(x, y, angle, cells, grid_points);

    for (int i = 0; i < cells; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if ((i + j) % 2 == 0)
            {
                renderable_push(&result, grid_points[i * 3 + j], grid_points[(i + 1) * 3 + j]);
                renderable_push(&result, grid_points[i * 3 + j], grid_points[i * 3 + (j + 1)]);
                renderable_push(&result, grid_points[(i + 1) * 3 + j], grid_points[(i + 1) * 3 + (j + 1)]);
                renderable_push(&result, grid_points[i * 3 + (j + 1)], grid_points[(i + 1) * 3 + (j + 1)]);

                renderable_push(&result, grid_points[i * 3 + j], grid_points[(i + 1) * 3 + (j + 1)]);
                renderable_push(&result, grid_points[(i + 1) * 3 + j], grid_points[i * 3 + (j + 1)]);
            }
            else
            {
            }
        }
    }

    renderable_push(&result, grid_points[0], grid_points[cells * 3]);
    renderable_push(&result, grid_points[0], grid_points[2]);
    renderable_push(&result, grid_points[cells * 3], grid_points[cells * 3 + 2]);
    renderable_push(&result, grid_points[2], grid_points[cells * 3 + 2]);

    return result;
}

double renderable_get_barrier_points(double x, double y, double angle, double width, double height, Vector2D *out)
{
    const double hw = width / 2;
    const double hh = height / 2;

    out[0] = (Vector2D){-hw, -hh};
    out[1] = (Vector2D){-hw, hh};
    out[2] = (Vector2D){hw, hh};
    out[3] = (Vector2D){hw, -hh};

    for (int i = 0; i < 4; i++)
    {
        out[i] = vector_add(vector_rotate(out[i], angle), (Vector2D){x, y});
    }

    return sqrt(hw * hw + hh * hh);
}

Renderable renderable_get_barrier(double x, double y, double angle, double width, double height)
{
    Renderable result = renderable_new(palette_get_r(Rust), //
                                       palette_get_g(Rust), //
                                       palette_get_b(Rust), //
                                       x, y);

    Vector2D grid_points[4];
    result.radius = renderable_get_barrier_points(x, y, angle, width, height, grid_points);

    for (int i = 0; i < 4; i++)
    {
        int i1 = (i + 1) % 4;
        renderable_push(&result, grid_points[i], grid_points[i1]);
    }

    renderable_push(&result, grid_points[0], grid_points[2]);
    renderable_push(&result, grid_points[1], grid_points[3]);

    return result;
}