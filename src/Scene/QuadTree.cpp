#include "QuadTree.h"

#include <valarray>

Node::Node(int x, int z, unsigned int size) : x(x), z(z), size(size), centerX(x + size / 2), centerZ(z + size / 2) {

}

Node::~Node() {

}

unsigned int QuadTree::getSize() {
    return size;
}

