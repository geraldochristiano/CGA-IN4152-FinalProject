#version 450

layout (location = 1) uniform samplerCube skybox;

in vec3 texCoords;

out vec4 fragColor;

void main()
{    
    fragColor = texture(skybox, texCoords);
}