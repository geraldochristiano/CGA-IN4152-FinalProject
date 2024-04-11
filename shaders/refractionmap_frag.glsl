#version 450

uniform samplerCube skybox;
uniform vec3 viewPos;
uniform float IOR = 1.00 / 1.52; // Air to glass

in vec3 fragNormal;
in vec3 fragPosition;

out vec4 fragColor;

void main()
{             
    vec3 I = normalize(fragPosition - viewPos);
    vec3 R = refract(I, normalize(fragNormal), IOR);
    // see skybox_vert.glsl for why X axis is inverted
    fragColor = vec4(texture(skybox, vec3(-R.x, R.yz)).rgb, 1.0);
}  