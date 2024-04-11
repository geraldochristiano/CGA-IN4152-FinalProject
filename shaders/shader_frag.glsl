#version 450

layout(std140) uniform Material // Must match the GPUMaterial defined in src/mesh.h
{
    vec3 kd;
	vec3 ks;
	float shininess;
	float transparency;
};

uniform sampler2D colorMap;
uniform bool hasTexCoords;
uniform bool useMaterial;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
    const vec3 normal = normalize(fragNormal);

    if (hasTexCoords)       { fragColor = vec4(texture(colorMap, fragTexCoord).rgb, 1);}
    else if (useMaterial)   { fragColor = vec4(kd, 1);}
    else                    { fragColor = vec4(normal, 1); } // Output color value, change from (1, 0, 0) to something else
}
