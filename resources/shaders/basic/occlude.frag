#version 460

layout (early_fragment_tests) in;

layout (std430, binding = 0) buffer VisibilityBuffer {
    uint visible[];
};

in flat int instanceID; // Not interpolated

void main() {
    atomicAdd(visible[instanceID], 1);
}