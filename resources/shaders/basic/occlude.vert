#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 boxData;

out int instanceID;

uniform mat4 view;
uniform mat4 projection;

uniform float tileSize;

void main()
{
    // BoxData contains the (x, z) coordinates as xy. In addition, the low and (high - low) values are zw
    gl_Position = projection * view * vec4(boxData.x + tileSize * aPos.x, boxData.z + boxData.w * aPos.y, boxData.y + tileSize * aPos.z, 1.0);

    instanceID = gl_InstanceID;
}