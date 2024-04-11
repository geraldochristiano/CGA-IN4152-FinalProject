#version 450

layout(location = 0) uniform mat4 viewProjMatrix;

layout (location = 0) in vec3 position;

out vec3 texCoords;

void main()
{   
    // - Texture is designed to wrap around mesh, so it looks correct when seeing from outside the mesh, but
    //   since we see the texture from inside for skybox, the X axis is flipped, so we must invert X axis
    texCoords = vec3(-position.x, position.y, position.z);
    vec4 clipSpacePos = viewProjMatrix * vec4(position, 1.0);
    gl_Position = clipSpacePos.xyww;
} 