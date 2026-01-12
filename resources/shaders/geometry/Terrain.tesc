#version 410 core

layout (vertices = 4) out;

in int edges[];

void main()
{
    // pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 16 + (edges[0] & 1) * 16;
        gl_TessLevelOuter[1] = 16 + (edges[0] >> 1 & 1) * 16;
        gl_TessLevelOuter[2] = 16 + (edges[0] >> 2 & 1) * 16;
        gl_TessLevelOuter[3] = 16 + (edges[0] >> 3 & 1) * 16;

        gl_TessLevelInner[0] = 16;
        gl_TessLevelInner[1] = 16;
    }
}