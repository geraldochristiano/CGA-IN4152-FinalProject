#version 450
#define PI 3.14159265358979323;
layout(std140) uniform Material // Must match the GPUMaterial defined in src/mesh.h
{
    //material color?
    vec3 kd; //determined by fresnel
	vec3 ks; //determined by fresnel
	float shininess;
	float transparency; // same as alpha?

};

layout(location = 3) uniform sampler2D colorMap;
layout(location = 4) uniform bool hasTexCoords;
layout(location = 5) uniform bool useMaterial;
//
//layout(location = 6) uniform vec3 lightPos;
//layout(location = 7) uniform vec3 color; // Currently material color (albedo), not light color
//layout(location = 8) uniform float alpha; 
//layout(location = 9) uniform vec3 viewPos;
//layout(location = 10) uniform vec3 reflectivity;
//layout(location = 11) uniform float roughness;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;


//float roughness = 1 - shininess;
//vec3 reflectivity = ks;


//vec3 cookTorrance() 
//{
//    vec3 lightVector = normalize(lightPos - fragPosition);
//    vec3 normalVector = normalize(fragNormal);
//    float LN = dot((lightVector), normalVector);
//    if(LN <= 0.0f) {
//        return vec3(0.0f);
//    } else {
//        vec3 viewVector = normalize(viewPos - fragPosition);
//        vec3 halfway = normalize(lightVector + viewVector);
//        //Fresnel
//        vec3 fresnel = reflectivity + (vec3(1.0f) - reflectivity) * pow((1 - (dot(viewVector, halfway))), 5.0);
//        //float kd = 1 - ks;
//        float a = roughness*roughness;
//        float pi = PI;
//        float GGX = a/(pi * pow((pow(dot(normalVector, halfway), 2)*(roughness - 1) + 1), 2));
//        float G1 = (2*dot(halfway, normalVector)*(dot(viewVector, normalVector)))/(dot(viewVector, halfway));
//        float G2 = (2*dot(halfway, normalVector)*(dot(lightVector, normalVector)))/(dot(viewVector, halfway));
//        float G = min(1.0f, min(G1, G2));
//        vec3 kd = 1-fresnel;
//        vec3 spec = (fresnel*GGX*G)/(4*dot(viewVector, normalVector)*dot(normalVector, lightVector));
//        vec3 BRDF = spec*LN + kd*color*LN/pi;
//        //vec3 reflectVector = normalize(2*(LN)*fragNormal-(lightVector));
//        //float RV = max(dot(fragNormal, halfway), 0.0f);
//        //float spec = pow(RV, alpha);
//        return BRDF;
//    }
//
//
//}
    

void main()
{
    const vec3 normal = normalize(fragNormal);


    if (hasTexCoords)       { fragColor = vec4(texture(colorMap, fragTexCoord).rgb, 1);}
    else if (useMaterial)   { fragColor = vec4(kd, 1);}
    else                    { fragColor = vec4(fragNormal, 1); } // Output color value, change from (1, 0, 0) to something else
}
