#version 460 core
#extension GL_ARB_shader_draw_parameters : enable
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in mat4 model;

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
flat out uint DrawID;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTex;
    DrawID = gl_DrawID;
}