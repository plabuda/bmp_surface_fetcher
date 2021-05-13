#include "vector2d.h"
#include <math.h>

Vector2D vector_mul(const Vector2D a, double scale)
{
    return (Vector2D){a.x * scale, a.y * scale};
}

Vector2D vector_div(const Vector2D a, double scale)
{
    return vector_mul(a, 1 / scale);
}

Vector2D vector_negate(const Vector2D a)
{
    return vector_mul(a, -1);
}

Vector2D vector_add(const Vector2D a, const Vector2D b)
{
    return (Vector2D){a.x + b.x, a.y + b.y};
}

Vector2D vector_sub(const Vector2D a, const Vector2D b)
{
    return vector_add(a, vector_negate(b));
}

Vector2D vector_cast(const Vector2D a, const Vector2D b)
{
    return vector_resize(b, vector_dot(a, b));
}

double vector_length_squared(const Vector2D a)
{
    return a.x * a.x + a.y * a.y;
}

double vector_length(const Vector2D a)
{
    return sqrt(vector_length_squared(a));
}

double vector_angle(const Vector2D a)
{
    return atan2(a.y, a.x);
}

double vector_dot(const Vector2D a, const Vector2D b)
{
    return a.x * b.x + a.y * b.y;
}

Vector2D vector_rotate(const Vector2D a, double angle)
{
    double sn = sin(angle);
    double cs = cos(angle);

    return (Vector2D){a.x * cs - a.y * sn, a.x * sn + a.y * cs};
}

Vector2D vector_resize(const Vector2D a, double target)
{
    double len = vector_length(a);
    return len > 0 ? vector_mul(a, target / len) : (Vector2D){target, 0};
}

Vector2D vector_normal(const Vector2D a)
{
    return vector_resize(a, 1);
}