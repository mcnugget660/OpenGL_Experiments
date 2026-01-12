#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D normal;

void main()
{

    FragColor = texture(diffuse, TexCoords).rgba;
    if(FragColor.a < 0.5) // quick fix but no the best
    discard;
}