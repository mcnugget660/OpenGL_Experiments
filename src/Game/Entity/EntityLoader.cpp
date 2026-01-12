#include "EntityLoader.h"

EntityID EntityLoader::createEntity(glm::vec2 pos, glm::vec2 velocity, std::string type, Location* location) {
    JSON_OBJECT* js = ResourceLoader::getJSON("Entities/" + type + ".json");
    std::vector<JSON_OBJECT> atr = js->OBJECT_fields.at("Attributes");

    EntityID id = location->entityCount++;

    if(js->boolean_fields.at("hasPos").at(0)){
        location->ECS.entityPositions[id] = pos;
    }

    if(js->boolean_fields.at("hasMovement").at(0) && js->boolean_fields.at("hasPos").at(0)){
        location->ECS.entityMovement[id] = EntityMovementC{pos, velocity};
    }

    for(JSON_OBJECT& obj : atr) {
        std::string t = obj.string_fields.at("type").at(0);
        if(t == "Bounds"){
          //  location->ECS.entityBounds[id] = EntityBoundsC{0};
        } else if(t == "Explosion") {
            location->ECS.entityExplosion[id] = EntityExplosionC{obj.double_fields.count("time") ? obj.double_fields.at("time").at(0) : obj.integer_fields.at("time").at(0)};
        } else if(t == "Render"){
            std::string mode = obj.string_fields.at("mode").at(0);
            if(mode == "model") {
                location->entityModelInstances[obj.integer_fields.at("model").at(0)].push_back(id);
            } else if (mode == "sdf") {
                location->sdfInstances[obj.integer_fields.at("shape").at(0)].push_back(id);
            }
        }
    }
    return id;
}

void EntityLoader::addAttributeMovement(EntityMovementC component) {

}
