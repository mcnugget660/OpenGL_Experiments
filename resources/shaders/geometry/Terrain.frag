#version 410 core
out vec4 FragColor;

in vec3 worldCoords;
in float slope;

in vec3 TangentFragPos;
in vec3 TangentSunPos;
in vec3 TangentMoonPos;

// Real Textures
uniform sampler2D ground;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D groundNormal;
uniform sampler2D grassNormal;
uniform sampler2D rockNormal;

uniform float grassRadius = 60;

void main()
{
    float height = worldCoords.y;

    vec2 texCoord = worldCoords.xz / 5.0;

    float rockPercent = min(1, max(0, height - 70) / 30.0f);
    float grassPercent = min(1, max(0, 70 - height) / 30.0f);
    float groundPercent = 1 - rockPercent - grassPercent;

    FragColor = vec4(texture(rock, texCoord).rgb * rockPercent + texture(grass, texCoord).rgb * grassPercent + texture(ground, texCoord).rgb * groundPercent, 1.0);

    //FragColor = mix(FragColor, FragColor * vec4(.37, .37, .12, 1.0), cameraDistance / grassRadius);

    //FragColor = mix(texture(rock, texCoord), FragColor, slope); // Steepness - change later

    vec3 normal = texture(rockNormal, texCoord).rgb * rockPercent + texture(grassNormal, texCoord).rgb * grassPercent + texture(groundNormal, texCoord).rgb * groundPercent;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 lightDir = normalize(TangentSunPos - TangentFragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

    vec3 ambient = 0.2 * vec3(1, 1, 1);


    FragColor *= vec4(diffuse + ambient, 1.0);

}