#version 450

uniform mat4 viewProjMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalModelMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{
    fragPosition = (modelMatrix * vec4(position, 1)).xyz;
    gl_Position = viewProjMatrix * vec4(fragPosition, 1);
    
    fragNormal = normalize(normalModelMatrix * normal);
    fragTexCoord = texCoord;
}
