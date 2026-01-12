#ifndef FINALDAYONEARTH_ECS_H
#define FINALDAYONEARTH_ECS_H

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "EntityComponents.h"

typedef unsigned int ComponentID;

static ComponentID COMPONENT_POSITION = 0;
static ComponentID COMPONENT_MOVEMENT = 1;
static ComponentID COMPONENT_BOUNDS = 2;
static ComponentID COMPONENT_CASTER = 3;
static ComponentID COMPONENT_EXPLOSION = 4;

struct EntityComponentStorage {
    std::unordered_map<EntityID, glm::vec2> entityPositions;
    std::unordered_map<EntityID, EntityMovementC> entityMovement;
    std::unordered_map<EntityID, EntityBoundsC> entityBounds;
    std::unordered_map<EntityID, EntityCasterC> entityCaster;
    std::unordered_map<EntityID, EntityExplosionC> entityExplosion;
    std::unordered_map<ComponentID, void*> componentVectors = {
            {COMPONENT_POSITION, &entityPositions},
            {COMPONENT_MOVEMENT, &entityMovement},
            {COMPONENT_BOUNDS, &entityBounds},
            {COMPONENT_CASTER, &entityCaster},
            {COMPONENT_EXPLOSION, &entityExplosion}
    };
};

#endif //FINALDAYONEARTH_ECS_H
