
#include "bike.h"
#include "acceleration.h"
#include "dot.h"
#include "palette.h"
#include "position.h"
#include "stick.h"
#include "vector2d.h"
#include "wheel.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void bike_render_offset(Bike *b, SDL_Renderer *r, double x, double y)
{
    Dot *dots = b->v->dots;
    Vector2D front = dots[b->front_index].position;
    Vector2D back = dots[b->back_index].position;

    Vector2D forward = vector_sub(front, back);

    palette_set_color(Metal_Dark);

    Vector2D wheel_forward = vector_resize(vector_rotate(forward, b->turn_angle * acos(-1) / 180.0), 15);
    Vector2D wheel_front = vector_add(front, wheel_forward);
    Vector2D wheel_back = vector_sub(front, wheel_forward);

    SDL_RenderDrawLine(r, wheel_front.x + x, wheel_front.y + y, wheel_back.x + x, wheel_back.y + y);

    Vector2D wheel_backward = vector_resize(forward, 15);
    Vector2D wheel_bfront = vector_add(back, wheel_backward);
    Vector2D wheel_bback = vector_sub(back, wheel_backward);

    SDL_RenderDrawLine(r, wheel_bfront.x + x, wheel_bfront.y + y, wheel_bback.x + x, wheel_bback.y + y);
}

void bike_render(Bike *b, SDL_Renderer *r)
{
    bike_render_offset(b, r, 0, 0);
}

void bike_steer(Bike *b, int turn, int speed)
{
    b->speed_force = speed;
    b->turn_force = turn;
}

float _update_angle(Sint8 turn_force, float turn_angle, float angle_limit)
{
    if (turn_force == 0 && turn_angle <= 8 && turn_angle >= -8)
    {
        turn_angle = 0;
    }

    if (turn_angle > 0 && turn_force <= 0)
    {
        turn_force -= 1;
    }
    else if (turn_angle < 0 && turn_force >= 0)
    {
        turn_force += 1;
    }

    turn_angle += turn_force * 7;

    turn_angle = turn_angle > angle_limit ? angle_limit : (turn_angle < -angle_limit ? -angle_limit : turn_angle);

    return turn_angle;
}

void _bike_update_angle(Bike *b, int angle_limit)
{
    b->turn_angle = _update_angle(b->turn_force, b->turn_angle, angle_limit);
}

int _bike_get_angle_limit_from_speed(float speed)
{
    return speed > 0 ? 30 - speed : 30 + speed;
}

float _bike_update_velocity(Dot *front, Dot *back, int speed_force)
{
    Vector2D forward = vector_normal(vector_sub(front->position, back->position));

    return wheel_accelerate(&back->old_position, &back->position, //
                            &forward,                             //
                            speed_force * 1.2, 15, -4);           //
}

void bike_update(Bike *b)
{
    Dot *dots = b->v->dots;
    Dot *front = dots + b->front_index;
    Dot *back = dots + b->back_index;

    float speed = _bike_update_velocity(front, back, b->speed_force);
    const int angle_limit = _bike_get_angle_limit_from_speed(speed);
    _bike_update_angle(b, angle_limit);
}

void lock_dot_to_angle(Dot *d, Vector2D normal)
{
    // Calculate the velocity that the dot will take
    Vector2D velocity = vector_sub(d->position, d->old_position);

    // Calculate the "valid" part of the velocity
    Vector2D correct_velocity = vector_cast(velocity, normal);

    Vector2D correction = vector_sub(correct_velocity, velocity);
    d->gravity = correction;
}

// set the "gravity" of each wheel to a vector that locks the velocity to the angle
void bike_constrain(Bike *b)
{
    Dot *dots = b->v->dots;
    Dot *front = dots + b->front_index;
    Dot *back = dots + b->back_index;

    // normalized forward vector of the bike
    Vector2D forward = vector_normal(vector_sub(front->position, back->position));
    // forward = vector_rotate(forward, 90.0 * acos(-1) / 180.0);
    // Normalized forward vector of the turned front wheel
    Vector2D front_wheel_forward = vector_rotate(forward, b->turn_angle * acos(-1) / 180.0);

    lock_dot_to_angle(front, front_wheel_forward);
    lock_dot_to_angle(back, forward);
}

//////////////////ECS//////////////////

void _bike_steer(ecs_iter_t *it)
{
    Bike_Component *b = ecs_column(it, Bike_Component, 1);
    Stick_Component *s = ecs_column(it, Stick_Component, 2);
    ecs_entity_t *e = it->entities;

    for (int i = 0; i < it->count; i++)
    {
        const Position *p_first = ecs_get(it->world, s[i].first, Position);
        const OldPosition *Op_first = ecs_get(it->world, s[i].first, OldPosition);
        const Position *p_second = ecs_get(it->world, s[i].last, Position);
        Wheel *w_back = ecs_get_mut(it->world, s[i].first, Wheel, NULL);
        Wheel *w_front = ecs_get_mut(it->world, s[i].last, Wheel, NULL);
        Acceleration *a = ecs_get_mut(it->world, s[i].first, Acceleration, NULL);

        // Actual system goes here
        // Get speed
        const Vector2D forward = vector_normal(vector_sub(*p_second, *p_first));
        const Vector2D velocity = vector_sub(*p_first, *Op_first);
        float speed = vector_dot(velocity, forward);
        // calculate angle limit based on speed
        const int angle_limit = _bike_get_angle_limit_from_speed(speed);
        // calculate angle
        b[i].turn_angle = _update_angle(b[i].turn_force, b[i].turn_angle, angle_limit);
        // apply angles to both wheels
        *w_back = forward;
        *w_front = vector_rotate(forward, b[i].turn_angle * acos(-1) / 180.0);
        // apply Acceleration to back wheel
        a[i] = (Acceleration){15, -4, b[i].speed_force * 1.2};

        ecs_modified(it->world, s[i].first, Wheel);
        ecs_modified(it->world, s[i].last, Wheel);
        ecs_modified(it->world, s[i].first, Acceleration);
    }
}

ecs_entity_t bike_add_steering_system(ecs_world_t *world, ecs_entity_t phase)
{
    ECS_COMPONENT_DEFINE(world, Bike_Component);
    ECS_COMPONENT_DEFINE(world, Stick_Component);
    ECS_COMPONENT_DEFINE(world, Wheel);
    ECS_COMPONENT_DEFINE(world, Acceleration);
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_SYSTEM(world, _bike_steer, phase, Bike_Component, [in] Stick_Component);
    return _bike_steer;
}