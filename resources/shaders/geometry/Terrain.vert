#version 410 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in int aEdges;
layout (location = 2) in mat4 modelMatrix;

out int edges;

void main()
{
    gl_Position = modelMatrix * vec4(aPos.x, 0.0f, aPos.y, 1.0);
    edges = aEdges;
}