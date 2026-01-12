#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 worldCoords;
layout (location = 2) in float rotation;

out vec3 normal;
out vec3 location;
out vec3 color;
out float cameraDistance;

uniform mat4x4 viewMatrix;
uniform mat4x4 projectionMatrix;

uniform vec3 viewVector;

uniform sampler2D groundHeight;
uniform sampler2D groundNormal;
uniform sampler2D wind;

uniform float heightMapSize;

uniform float tilt = .09f; // Displacement of tip vertically from the base
uniform float bend = 0.02f; // Displacement of midpoint away from line between base and tip
uniform float grassLength = 0.1f;
uniform float grassWidth = 0.011f;

uniform float time; // In seconds
uniform vec2 windDirection = normalize(vec2(1.0f, 0.0f));
uniform float windSpeed = 0.1f; // In meters per second
uniform float stiffness = 1.0f;

uniform vec3 darkGreen = vec3(.37, .37, .12);
uniform vec3 lightGreen = vec3(.35, .35, .1);

uniform float roundness = 1.f; // 1 is 45

uniform vec3 cameraPos;

void main()
{

    vec2 textureCoords = (worldCoords.yx + 0.5f) / heightMapSize;

    float verticalOffset = float(texture(groundHeight, textureCoords));

    vec3 up = normalize(texture(groundNormal, textureCoords).xyz * 2.0f - 1.0f);

    vec3 forward = normalize(vec3(cos(rotation), 0, sin(rotation)));

    vec3 orthogonal = normalize(cross(up, forward));


    // Color
    color = mix(darkGreen, lightGreen, smoothstep(0.0f, 1.0f, aPos.y));


    //Wind

    //  Find change in x, y of wind by windSpeed * direction * time, then convert to texture coordinates
    float windStrength = float(texture(wind, textureCoords - (windDirection * time * windSpeed / heightMapSize).yx)) / stiffness;

    float tipXZLength = sqrt(grassLength * grassLength - tilt * tilt);

    vec3 tip = tilt * up + tipXZLength * forward;


    // Normal to a perfectly straight blade
    vec3 bladeNormal = normalize(cross(tip, orthogonal));

    vec3 midpoint = tip * 0.5 + bladeNormal * bend;

    vec3 point = 2 * (1 - aPos.y) * aPos.y * midpoint + aPos.y * aPos.y * tip;

    location = point + aPos.x * grassWidth * orthogonal + vec3(worldCoords.x, verticalOffset, worldCoords.y);


    // The derivative will yeild a vector tangential to the point on the curve
    normal = normalize(cross((2 * tip - 4 * midpoint) * aPos.y + 2 * midpoint, orthogonal));

    // Push normal outwards
    normal = normalize(normal + roundness * 2 * orthogonal * (aPos.x - 0.5));


    // TODO : Push verticies slightly when blade appears very thin
    float dotProduct = dot(forward.xz, viewVector.xz);

    // Will affect blade less and less as  the blade becomes more orthagonal
    float giveUp = 0 * smoothstep(0.0, 1.0, abs(dotProduct));

    float moveAmount = grassWidth * 0.3 * (1.0 - abs(dotProduct)) * (aPos.x - 0.5) * 2 * sign(dotProduct);

    cameraDistance = distance(location, cameraPos);

    //gl_Position = projectionMatrix * (vec4(giveUp * moveAmount, 0.0f, 0.0f, 0.0f) + viewMatrix * vec4(location, 1.0));
    gl_Position = projectionMatrix * viewMatrix * vec4(location, 1.0);
}