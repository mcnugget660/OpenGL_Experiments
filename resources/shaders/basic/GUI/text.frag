#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 color;

uniform float thickness = 0.5;// Controls the edge location
uniform float softness = 0.015;// Controls the outline crispness

uniform bool outline;
uniform vec3 outlineColor = vec3(0, 0, 0);
uniform float outlineDepth = 0.075;// Extends into normal width
uniform float outlineSmoothness = 0.0175;// Blend region between border color and main color

uniform bool glow;
uniform float glowWidth = 0.2;// Expands from normal width
uniform vec3 glowColor = vec3(1, 0, 0);

uniform bool shadow = true;
uniform vec2 shadowOffset;// In texels (pixels)
uniform vec3 shadowColor = vec3(0, 0, 0);
uniform float shadowBlur;

void main()
{
    float glyphDistance = texture(text, TexCoords).r;// texture is single channel transparency

    vec4 displayColor = vec4(color, 1.0);

    if (outline){
        // Smooths the border between the outline and filll
        float outline = smoothstep(1.0 - thickness + outlineDepth - outlineSmoothness, 1.0 - thickness + outlineDepth + outlineSmoothness, glyphDistance);
        displayColor = mix(vec4(outlineColor, 1.0), displayColor, outline);
    }

    //Smooth outline
    float alpha = smoothstep(1.0 - thickness - softness, 1.0 - thickness + softness, glyphDistance);
    displayColor.a = alpha;

    if (glow){
        float fade = smoothstep(1.0 - thickness - glowWidth, 1.0 - thickness, glyphDistance);
        //Blend between the rapidly diminishing main color and the slowly receding glow color
        displayColor = vec4(mix(glowColor, displayColor.xyz, alpha), fade);
    }

    if (shadow){
        float shadowDist = texture(text, TexCoords + shadowOffset).r;
        float b = smoothstep(1.0 - thickness - shadowBlur, 1.0 - thickness + shadowBlur, shadowDist);
        //Render the shadow underneath the character
        displayColor = mix(vec4(shadowColor, b), displayColor, displayColor.a);
    }

    FragColor = displayColor;
}
