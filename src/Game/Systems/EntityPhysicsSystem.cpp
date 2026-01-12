#include "EntityPhysicsSystem.h"
#include <iostream>

//WIP
/*
void EntityPhysicsSystem::execute(Location *location, double deltaTime) {

    location->localObjects = (location->resourseOnlyGridStructure);

    // Make sure to actually set the components to their new value

    // Update Entity Positions
    for(auto& [type, velocities] : location->ECS.entityVelocities){
        auto& positions = location->ECS.entityPositions[type];

        for(int i = 0; i < positions.size(); i++) {
            auto& pos = positions[i];
            auto& vel = velocities[i];

            pos += (float) deltaTime * vel;
            auto& bounds = location->ECS.entityBounds[id];

            double boxSize = (location->size / location->localObjects.size());
            int boxesPerRow = location->localObjects.size();
            for(int i = 0; i < 4; i++) {
                int x = (location->ECS.entityPositions[id].x + 1.01 * bounds.radius * (-1 + 2*(i % 2)))/ boxSize;
                int y = (location->ECS.entityPositions[id].y + 1.01 * bounds.radius * (-1 + 2*(i > 1)))/ boxSize;
                location->localObjects.at(x).at(y).second.emplace_back(id);
            }
        }
    }

    for(auto& column : location->localObjects) {
        for(auto& box : column) {
            std::vector<std::vector<std::pair<unsigned int, glm::vec3>>> forceVector(box.second.size());
            for(EntityID id : box.second) {
                glm::vec2& ePos = location->ECS.entityPositions[id];
                for(unsigned int& rId : box.first) {
                    // Resolve Overlap
                    EntityBoundsC &eBound = location->ECS.entityBounds[id];
                    ResourceData &rInfo = location->resourceInfo[resource.first];

                    glm::vec2 displacement = ePos - resource.second;

                    if(displacement.length() < rInfo.radius){
                        ePos += (float) (rInfo.radius - displacement.length()) * displacement / (float) displacement.length();
                    }
                    // add force vector
                }
                for(EntityID other : box.second) {
                    if(id != other) {
                        //Resolve overlap

                        //Deal with forces
                    }
                }
            }
        }
    }

    // Resolve Entity Collisions
    for(auto& [id, entityMovementData] : location->ECS.entityMovement){
        EntityBoundsC &entityBounds = location->ECS.entityBounds.at(id);


        int index = 0;
        while(index < location->resourceLocations.size()){
            ResourceData &data = location->resourceInfo[index];
            for(auto const &resourcePos : location->resourceLocations[index]){
                float dist = glm::distance(entityPos, resourcePos);
                if(entityBounds.radius + data.radius > dist) {
                    location->ECS.entityPositions[id] = entityPos + entityMovementData.direction * (float) (dist - (entityBounds.radius + data.radius));
                }
            }
            index++;
        }
    }
}
*/