#version 410 core

layout (quads, equal_spacing, ccw) in;

out vec3 worldCoords;
out float slope;

out vec3 TangentFragPos;
out vec3 TangentSunPos;
out vec3 TangentMoonPos;

uniform mat4x4 viewMatrix;
uniform mat4x4 projectionMatrix;

uniform sampler2D grassColor;
uniform sampler2D heightMap;
uniform sampler2D tangentMap;
uniform sampler2D bitangentMap;
uniform float heightMapSize;

uniform vec3 sunPos;
uniform vec3 moonPos;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    // X is the row, z is the column

    // lookup texel at patch coordinate for height
    vec2 texCoords = (p.zx + 0.5f) / heightMapSize; // World coordinates divided by heightmap size

    float height = float(texture(heightMap, texCoords));

    vec3 tangent = normalize(texture(tangentMap, texCoords).rgb * 2.0f - 1.0f);
    vec3 bitangent = normalize(texture(bitangentMap, texCoords).rgb * 2.0f - 1.0f);

    vec3 normal = normalize(cross(tangent, bitangent));


    worldCoords = vec3(p.x, height, p.z);
    slope = normal.y;

    mat3 TBN = transpose(mat3(tangent, bitangent, normal));
    TangentSunPos = TBN * sunPos;
    TangentMoonPos = TBN * moonPos;
    TangentFragPos = TBN * worldCoords;

    gl_Position = projectionMatrix * viewMatrix * vec4(worldCoords, p.w);
}