#version 450

uniform vec3 viewPos;
uniform samplerCube skybox;

in vec3 fragNormal;
in vec3 fragPosition;

out vec4 fragColor;

void main()
{             
    vec3 I = normalize(fragPosition - viewPos);
    vec3 R = reflect(I, normalize(fragNormal));
    // see skybox_frag.glsl for why X axis is inverted
    fragColor = vec4(texture(skybox, vec3(-R.x, R.yz)).rgb, 1.0);
}