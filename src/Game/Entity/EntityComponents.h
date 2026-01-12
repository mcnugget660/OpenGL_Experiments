#ifndef FINALDAYONEARTH_ENTITYCOMPONENTS_H
#define FINALDAYONEARTH_ENTITYCOMPONENTS_H

typedef unsigned int EntityID;

struct EntityMovementC{
    glm::vec2 pos;
    glm::vec2 direction;
    float speed = 0.0f;
};

struct EntityBoundsC{
    double radius = 0;
};

struct EntityCasterC{
    uint maxMana = 0;
    double mana = 0;
    double manaRegenSpeed = 0;
};

struct EntityExplosionC{
    double timer = 0;
};

#endif //FINALDAYONEARTH_ENTITYCOMPONENTS_H
