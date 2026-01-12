#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 location;
in vec3 color;
in float cameraDistance;

uniform vec3 sunPos;
uniform vec3 moonPos;

uniform float grassRadius = 20;
uniform float grassFadeStart = 8;

void main()
{
    vec3 lightDir = normalize(sunPos - location);

    float diff = abs(dot(normal, lightDir));
    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

    vec3 ambient = 0.2 * vec3(1, 1, 1);

    float fade = smoothstep(0, 1, clamp(grassRadius - cameraDistance, 0, grassRadius - grassFadeStart) / (grassRadius - grassFadeStart));

    FragColor = vec4(color * (diffuse + ambient), fade);

}