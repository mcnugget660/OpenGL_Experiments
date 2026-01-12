#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 pos;
out vec3 opos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    opos = (mat4(model[0][0], model[0][1], model[0][2], model[0][3],
                model[1][0], model[1][1], model[1][2], model[1][3],
                model[2][0], model[2][1], model[2][2], model[2][3],
                0, 0, 0, model[3][3]) * vec4(aPos, 1.0)).xyz;
    vec4 p = model * vec4(aPos, 1.0);
    pos = p.xyz;
    gl_Position = projection * view * p;
}