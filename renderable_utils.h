#pragma once
#include "renderable.h"

double renderable_get_checkpoint_points(double x, double y, double angle,
                                        int cells, Vector2D *grid_points);
Renderable renderable_get_checkpoint(double x, double y, double angle);

double renderable_get_barrier_points(double x, double y, double angle,
                                     double width, double height,
                                     Vector2D *out);
Renderable renderable_get_barrier(double x, double y, double angle,
                                  double width, double height);