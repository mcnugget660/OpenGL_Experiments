#ifndef FINALDAYONEARTH_CONTIGUOUSMEMORYSTORAGE_H
#define FINALDAYONEARTH_CONTIGUOUSMEMORYSTORAGE_H

#include <vector>
#include <typeinfo>
#include <iostream>
#include <cstring>
#include <list>

template<typename T>
class ContiguousMemoryStorage {
public:
    explicit ContiguousMemoryStorage(u_int16_t maxComponents) : maxComponents(maxComponents) {
        components.reserve(maxComponents);
    }

    void addData(T *pointer, u_int16_t amount) {
        if (components.size() + amount > maxComponents) {
            std::cout << amount << " components of " << typeid(T).name() << " type can not be added to CompoenntMemoryStorage\n";
            return;
        }

        // Add data to the end of the vector
        memcpy(components.begin() + amount * sizeof(T), pointer, amount * sizeof(T));
    }

    void removeIndex(int index) {
        // Shrinks the array to remain contiguous
        std::swap(components[index], components.back());
        components.pop_back();
    }

private:
    u_int16_t maxComponents = 0;

    std::vector<T> components;
};

#endif //FINALDAYONEARTH_CONTIGUOUSMEMORYSTORAGE_H
