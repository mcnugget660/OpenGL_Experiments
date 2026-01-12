#ifndef FINALDAYONEARTH_LOCATIONGENERATOR_H
#define FINALDAYONEARTH_LOCATIONGENERATOR_H

#include "Location.h"

class LocationGenerator {
public:
    static Location *generateRandomLocation(Difficulty difficulty, Biome biome, Size size, int seed);
};


#endif //FINALDAYONEARTH_LOCATIONGENERATOR_H
