#ifndef A_TRIANGLEMESHTEX_H
#define A_TRIANGLEMESHTEX_H

#include "TriangleMesh.h"

class TriangleMeshTEX : public TriangleMesh {
public:
    TriangleMeshTEX(std::vector <Vertex5f> vertices, std::vector<unsigned int> indices);

};


#endif //A_TRIANGLEMESHTEX_H
