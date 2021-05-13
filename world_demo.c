#include "world_demo.h"
#include "globals.h"

extern Globals G;

World *_world_demo_world;
int _world_demo_turn, _world_demo_timeout;
int _world_demo_drive;

void _world_demo_randomise()
{
    if (_world_demo_drive)
    {
        _world_demo_drive = 0;

        _world_demo_turn = (rand() % 2) * 2 - 1;
        _world_demo_timeout = 5 + rand() % 40;
    }
    else
    {

        _world_demo_drive = 1;

        _world_demo_turn = 0;
        _world_demo_timeout = 30 + rand() % 90;
    }
}

void world_demo_init()
{
    _world_demo_world = world_new(0, 0);
    _world_demo_randomise();
}

void world_demo_frame()
{
    world_steer(_world_demo_world, _world_demo_turn, 1);
    world_update(_world_demo_world, NULL, NULL);
    world_render(G.renderer, _world_demo_world);

    if (_world_demo_timeout-- < 0)
    {
        _world_demo_randomise();
    }
}
