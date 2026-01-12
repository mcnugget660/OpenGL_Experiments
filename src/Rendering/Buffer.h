#ifndef FINALDAYONEARTH_BUFFER_H
#define FINALDAYONEARTH_BUFFER_H

#include <cstdlib>
#include <climits>
#include <vector>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include "glad/glad.h"


const uint DEFAULT_BUFFER_SIZE = 256;
const uint MIN_BUFFER_SIZE = 32;
const uint DEFAULT_MAX_BUFFER_SIZE = -1;

template <typename T>
struct Buffer{
    unsigned int buffer = -1;
    unsigned int maxSize = MIN_BUFFER_SIZE;
    unsigned int length = DEFAULT_BUFFER_SIZE;
    unsigned int dataCount = 0;
    unsigned int dataSize = sizeof(T);

    explicit Buffer(unsigned int maxSize = DEFAULT_MAX_BUFFER_SIZE, GLenum usage = GL_DYNAMIC_DRAW) : maxSize(maxSize) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, dataSize * length, nullptr, usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ~Buffer() {
        glDeleteBuffers(1, &buffer);
    }

    // There are 100% unneeded operations

    // Copied
    void setBufferSize(int newSize, int preserve) {
        assert(buffer != -1 && preserve <= dataCount && preserve <= newSize && newSize >= 0);

        if (newSize == length) return; // No size change needed

        GLsizeiptr copySize = dataSize * preserve;
        GLsizeiptr growSize = dataSize * newSize;

        // Step 1: Allocate a temporary buffer
        unsigned int tempBuffer = -1;

        if (preserve > 0) {
            glGenBuffers(1, &tempBuffer);
            glBindBuffer(GL_COPY_WRITE_BUFFER, tempBuffer);
            glBufferData(GL_COPY_WRITE_BUFFER, copySize, nullptr, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_COPY_READ_BUFFER, buffer);

            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, copySize);
        }

        // Step 3: Reallocate original buffer with new size
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, growSize, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (preserve > 0) {
            // Step 4: Copy data back from temp to original buffer
            glBindBuffer(GL_COPY_READ_BUFFER, tempBuffer);
            glBindBuffer(GL_COPY_WRITE_BUFFER, buffer);

            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, copySize);

            // Step 5: Delete temp buffer
            glDeleteBuffers(1, &tempBuffer);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
        }

        // Step 6: Update size
        dataCount = preserve;
        length = newSize;
    }


    int growBuffer(int number, int preserve) {
        if(number > length && length != maxSize){
            setBufferSize(std::min((unsigned int) std::pow(2, std::ceil(std::log2(number))), maxSize), preserve);
        }
        return length - preserve;
    }

    bool setData(std::vector<T> &data, unsigned int offset = 0) {
        if(offset > dataCount) {
            std::cout << "Error : offset exceeds buffer data bounds\n";
            return false;
        }

        if(growBuffer(data.size() + offset, offset) < data.size()) {
            std::cout << "Error : buffer too small for data";
            return false;
        }
        // Shrink Buffer
        if(data.size() + offset < length / 4 && length != MIN_BUFFER_SIZE)
            setBufferSize(std::max(MIN_BUFFER_SIZE, length / 2), offset);

        // Send data to GPU
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferSubData(GL_ARRAY_BUFFER, dataSize * offset, dataSize * data.size(), data.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        dataCount = data.size() + offset;

        return true;
    }

    bool setDataExactSize(std::vector<T> &data, unsigned int offset = 0) {
        if(offset > dataCount) {
            std::cout << "Error : offset exceeds buffer data bounds\n";
            return false;
        }

        if(length != data.size() + offset)
            setBufferSize(data.size() + offset, offset);

        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferSubData(GL_ARRAY_BUFFER, dataSize * offset, dataSize * data.size(), data.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        dataCount = data.size() + offset;
        return true;
    }

    bool addData(std::vector<T> &data) {
        if(growBuffer(data.size() + dataCount, dataCount) < data.size()) {
            std::cout << "Error : buffer too small for data";
            return false;
        }

        // Send data to GPU
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferSubData(GL_ARRAY_BUFFER, dataSize * dataCount, dataSize * data.size(), data.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        dataCount += data.size();

        return true;
    }


};




#endif //FINALDAYONEARTH_BUFFER_H
