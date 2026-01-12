#ifndef FINALDAYONEARTH_RESOURCE_H
#define FINALDAYONEARTH_RESOURCE_H

#endif //FINALDAYONEARTH_RESOURCE_H

#include "../Utility/ResourceLoader.h"

// Can be either stored on the CPU or GPU
// The primary purpose is to prevent duplicate instances of effectively static data

class Resource {
    friend class ResourceLoader;
public:
    void operator delete( void * ) {
        std::cout << "delete should not be called on a Resource - use ResourceLoader::free__ResourceName__()\n";
    }
};