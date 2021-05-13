#pragma once
#include "flecs.h"

ecs_entity_t renderer_add_rendering_system(ecs_world_t *world,
                                           ecs_entity_t phase);