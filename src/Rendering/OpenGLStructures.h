#ifndef FINALDAYONEARTH_OPENGLSTRUCTURES_H
#define FINALDAYONEARTH_OPENGLSTRUCTURES_H

#include "glad/glad.h"

struct DrawArraysIndirectCommand {
    GLuint count;
    GLuint instanceCount;
    GLuint first;
    GLuint baseInstance;
};

typedef  struct {
    uint  count;
    uint  instanceCount;
    uint  firstIndex;
    int  baseVertex;
    uint  baseInstance;
} DrawElementsIndirectCommand;



struct ComputeShaderIndirectCommand {
    uint num_groups_x;
    uint num_groups_y;
    uint num_groups_z;
};

#endif //FINALDAYONEARTH_OPENGLSTRUCTURES_H
