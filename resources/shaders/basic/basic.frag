#version 330 core
out vec4 FragColor;

uniform vec3 color = vec3(255, 0, 0);// Default red

void main()
{
    FragColor = vec4(color, 1.0);
}