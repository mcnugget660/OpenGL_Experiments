#ifndef FINALDAYONEARTH_LOCATIONSYSTEM_H
#define FINALDAYONEARTH_LOCATIONSYSTEM_H

#include "../Location/Location.h"

class LocationSystem {
public:
    // DeltaTime is in frameTime not game time
    virtual void execute(Location *location, double deltaTime) = 0;
};

#endif //FINALDAYONEARTH_LOCATIONSYSTEM_H
