#ifndef FINALDAYONEARTH_ENTITYLOADER_H
#define FINALDAYONEARTH_ENTITYLOADER_H

#include <vector>
#include "../Location/Location.h"

class EntityLoader {
public:
    //add parameter for extra data
    static EntityID createEntity(glm::vec2 pos, glm::vec2 direction, std::string type, Location *location);
private:
    static void addAttributeMovement(EntityMovementC component);
};


#endif //FINALDAYONEARTH_ENTITYLOADER_H
