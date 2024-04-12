#version 450 core


// Global variables for lighting calculations
//layout(location = 1) uniform vec3 viewPos;
layout(location = 1) uniform vec3 lightPos;
layout(location = 2) uniform vec3 color;
layout(location = 3) uniform float alpha;
layout(location = 4) uniform vec3 viewPos;
layout(location = 5) uniform vec3 ks;
// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal

void main()
{
    vec3 lightVector = normalize(lightPos - fragPos);
    float LN = dot((lightVector), fragNormal);
    if(LN <= 0.0f) {
        outColor = vec4(0.0f);
    } else {
        vec3 viewVector = normalize(viewPos - fragPos);
        vec3 halfway = normalize(lightVector + viewVector);
        //vec3 reflectVector = normalize(2*(LN)*fragNormal-(lightVector));
        float RV = max(dot(fragNormal, halfway), 0.0f);
        float spec = pow(RV, alpha);
        outColor = vec4(color*ks*spec, 1.0);
    }
}