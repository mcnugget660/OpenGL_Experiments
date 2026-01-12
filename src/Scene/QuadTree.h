#ifndef A_TILE_H
#define A_TILE_H

struct Node {
    Node(int x, int z, unsigned int size);

    ~Node();

    unsigned int size{};
    int x{};
    int z{};
    int centerX{};
    int centerZ{};
};

class QuadTree {
public:

    QuadTree(unsigned int size) : size(size) {};

    unsigned int getSize();

private:
    unsigned int size;

protected:
};


#endif //A_TILE_H
