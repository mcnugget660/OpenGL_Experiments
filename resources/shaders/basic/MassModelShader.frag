#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in flat uint DrawID;

uniform sampler2DArray diffuse;
uniform int DrawIDOffset = 0;
uniform bool alphaDiscard = true;
void main()
{
    FragColor = texture(diffuse, vec3(TexCoords, DrawID + DrawIDOffset)).rgba;
    if(alphaDiscard && FragColor.a < .3) // quick fix but not the best
    discard;
}