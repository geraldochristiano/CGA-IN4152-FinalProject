#version 450

// Each light type have their own shader, chosen using preprocessor directive
// #define LIGHT_TYPE [POINT_LIGHT | DIRECTIONAL_LIGHT | SPOT_LIGHT]
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2
#define PI 3.14159265358979323;

//$define_string

layout (std140) uniform Material 
{
	vec3 kd;
	vec3 ks;
	float shininess;
	float transparency;
} material;
uniform bool useMaterial = false;
uniform bool useBlinnPhong = true;
uniform bool applyAmbientLighting;
uniform vec3 viewPos;
uniform bool hasTexCoords;
uniform sampler2D kdtex;
uniform sampler2D normaltex;
uniform sampler2D roughnesstex;
uniform float reflectivity;

uniform vec3 lightColor = vec3(1,1,1);
uniform vec3 lightIntensities = vec3(0, 0.5, 1.0);
#ifdef LIGHT_TYPE
	#if (LIGHT_TYPE == POINT_LIGHT) 
		uniform vec3 lightPosition;
	#elif (LIGHT_TYPE == DIRECTIONAL_LIGHT)
		uniform vec3 lightDirection;
	#elif (LIGHT_TYPE == SPOT_LIGHT)
		uniform vec3 lightPosition;
		uniform vec3 lightDirection;
		uniform float cutoffAngle;
	#endif
#else
	const vec3 defaultLightPosition = vec3(3,3,3);
#endif

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout (location = 0) out vec4 fragColor;

vec3 cookTorrance(vec3 lightVector, float roughness, vec3 reflectivity, vec3 color) 
{
	//roughness = roughness;
	//reflectivity = vec3(0.02f);
    //vec3 lightVector = normalize(lightPos - fragPosition);
    vec3 normalVector = normalize(fragNormal);

    
	if (true) {
        vec3 viewVector = normalize(viewPos - fragPosition);
        vec3 halfway = normalize(lightVector + viewVector);

		float LN = clamp(dot((lightVector), normalVector), 0.0001f, 1.0f);
		float VN = clamp(dot((viewVector), normalVector), 0.0001f, 1.0f);
		float HN = clamp(dot((halfway), normalVector), 0.0001f, 1.0f);
		float VH = clamp(dot((viewVector), halfway), 0.0001f, 1.0f);

        //Fresnel
        vec3 fresnel = reflectivity + (vec3(1.0f) - reflectivity) * pow((1.0f - (VH)), 5.0f);
        //float kd = 1 - ks;
        float a = roughness*roughness;
        float pi = PI;
        float GGX = (a*a)/(pi * pow((pow(HN, 2)*(a*a - 1) + 1), 2));
        //float G1 = (2*dot(halfway, normalVector)*(dot(viewVector, normalVector)))/(dot(viewVector, halfway));
        //float G2 = (2*dot(halfway, normalVector)*(dot(lightVector, normalVector)))/(dot(viewVector, halfway));
		float GSmith1 = VN/(VN * (1.0f - (a / 2.0f)) + (a / 2.0f));
		float GSmith2 = LN/(LN * (1.0f - (a / 2.0f)) + (a / 2.0f));
        //float G = min(1.0f, min(G1, G2));
		float G = GSmith1*GSmith2;
        vec3 kd = 1.0f - fresnel;
        vec3 spec = (fresnel*GGX*G)/(4.0f*VN*LN);
        vec3 BRDF = LN*((spec) + (kd*color)/pi);
        //vec3 reflectVector = normalize(2*(LN)*fragNormal-(lightVector));
        //float RV = max(dot(fragNormal, halfway), 0.0f);
        //float spec = pow(RV, alpha);
        return BRDF;
    }


}

void main()
{	
	if (!useMaterial){
		fragColor = vec4(1,1,1,1);
	} else {
		//Ambient
		vec3 ambientColor = vec3(0);
		if (applyAmbientLighting){
			ambientColor = (lightIntensities.r * lightColor) * material.kd;
		}
//		vec3 ambientColor = int(applyAmbientLighting) * (lightIntensities.r * lightColor) * material.kd;
		
		// Diffuse
		vec3 N = normalize(fragNormal);
		#ifdef LIGHT_TYPE
			#if (LIGHT_TYPE == POINT_LIGHT)
				vec3 L = normalize(lightPosition - fragPosition);
			#elif (LIGHT_TYPE == DIRECTIONAL_LIGHT)
				vec3 L = normalize (-lightDirection);
			#elif (LIGHT_TYPE == SPOT_LIGHT)
				vec3 L = normalize(lightPosition - fragPosition); 
			#endif
		#else 
			vec3 L = normalize(defaultLightPosition - fragPosition);
		#endif
		float diffuseStrength = max(dot(N, L), 0.0);
		vec3 diffuseColor = diffuseStrength * (lightIntensities.g * lightColor) * material.kd;

		// Specular
		vec3 V = normalize(viewPos - fragPosition);
		if (useBlinnPhong){
			vec3 H = normalize(L + V);
			vec3 specularColor = pow(max(dot(N, H), 0.0), material.shininess) * (lightIntensities.b * lightColor) * material.ks;
			if(dot(L, N) < 0) {
				specularColor = vec3(0.0f);
			}
			fragColor = vec4(ambientColor + diffuseColor + specularColor, material.transparency);
		} else {
			vec3 R = reflect(-L, N);
			float specularStrength = pow(max(dot(V, R),0.0), material.shininess);
			vec3 specularColor = specularStrength * (lightIntensities.b * lightColor) * material.ks;
			if(dot(L, N) < 0) {
				specularColor = vec3(0.0f);
			}
			fragColor = vec4(ambientColor + diffuseColor + specularColor, material.transparency);
		}

		//PBR should have no kd value
		if(hasTexCoords && length(material.kd) < 0.005f) {
			vec3 color = texture(kdtex, fragTexCoord).rgb;
			float roughness = texture(roughnesstex, fragTexCoord).r;
			fragColor = vec4(cookTorrance(L, roughness, vec3(reflectivity), color)*lightColor, 1.0f);
			
		}
	}
}

