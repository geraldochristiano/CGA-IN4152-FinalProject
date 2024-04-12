#version 450 core
#define PI 3.14159265358979323;



// Global variables for lighting calculations
layout(location = 1) uniform vec3 lightPos;
layout(location = 2) uniform vec3 color;
layout(location = 3) uniform float alpha;
layout(location = 4) uniform vec3 viewPos;
layout(location = 5) uniform vec3 reflectivity;
layout(location = 6) uniform float roughness;
//layout(location = 4) uniform sampler2D texShadow;

// Output for on-screen color
layout(location = 0) out vec4 fragColor;

// Interpolated output data from vertex shader
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;


void main()
{   //Cook torrance
    
    vec3 lightVector = normalize(lightPos - fragPosition);
    vec3 normalVector = normalize(fragNormal);
    float LN = dot((lightVector), normalVector);
    if(LN <= 0.0f) {
        fragColor = vec4(0.0f);
    } else {
        vec3 viewVector = normalize(viewPos - fragPosition);
        vec3 halfway = normalize(lightVector + viewVector);
        //Fresnel
        vec3 fresnel = reflectivity + (vec3(1.0f) - reflectivity) * pow((1 - (dot(viewVector, halfway))), 5.0);
        //float kd = 1 - ks;
        float a = roughness*roughness;
        float pi = PI;
        float GGX = a/(pi * pow((pow(dot(normalVector, halfway), 2)*(roughness - 1) + 1), 2));
        float G1 = (2*dot(halfway, normalVector)*(dot(viewVector, normalVector)))/(dot(viewVector, halfway));
        float G2 = (2*dot(halfway, normalVector)*(dot(lightVector, normalVector)))/(dot(viewVector, halfway));
        float G = min(1.0f, min(G1, G2));
        vec3 kd = 1-fresnel;
        vec3 ks = (fresnel*GGX*G)/(4*dot(viewVector, normalVector)*dot(normalVector, lightVector));
        vec3 BRDF = ks*LN + kd*color*LN/pi;
        //vec3 reflectVector = normalize(2*(LN)*fragNormal-(lightVector));
        //float RV = max(dot(fragNormal, halfway), 0.0f);
        //float spec = pow(RV, alpha);
        fragColor = vec4(BRDF, 1.0f);
    }

}