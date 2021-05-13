#pragma once

typedef struct {
  float x, y;
} Vector2D;

Vector2D vector_mul(const Vector2D a, double scale);
Vector2D vector_div(const Vector2D a, double scale);

Vector2D vector_negate(const Vector2D a);

Vector2D vector_add(const Vector2D a, const Vector2D b);
Vector2D vector_sub(const Vector2D a, const Vector2D b);

double vector_length_squared(const Vector2D a);
double vector_length(const Vector2D a);

double vector_angle(const Vector2D a);
double vector_dot(const Vector2D a, const Vector2D b);

Vector2D vector_cast(const Vector2D a, const Vector2D b);
Vector2D vector_resize(const Vector2D a, double target);
Vector2D vector_normal(const Vector2D a);

Vector2D vector_rotate(const Vector2D a, double angle);