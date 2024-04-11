#version 450

uniform mat4 viewProjMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalModelMatrix;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 fragNormal;
out vec3 fragPosition;

void main()
{
    fragNormal = normalize(normalModelMatrix * normal);
    fragPosition = vec3(modelMatrix * vec4(pos, 1.0));
    gl_Position = viewProjMatrix * vec4(fragPosition, 1.0);
}  