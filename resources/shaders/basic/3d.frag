#version 330 core
out vec4 FragColor;
in vec3 pos;
in vec3 opos;

uniform vec3 cameraPos;
uniform vec3 color;

uniform vec3 sunPos;
uniform vec3 moonPos;

void main()
{
    vec3 dir = normalize(pos - cameraPos);
    float distance = abs(length(cross(opos, dir)));
    if(distance > 1)
    discard;

    vec3 normal = normalize(-dir * sqrt(1 - distance * distance) + distance * normalize(opos + dot(-opos, dir) * dir));

    vec3 lightDir = normalize(sunPos - pos);

    float diff = abs(dot(normal, lightDir));
    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

    vec3 ambient = 0.2 * vec3(1, 1, 1);

    FragColor = vec4(color * (diffuse + ambient), 1.0);
}