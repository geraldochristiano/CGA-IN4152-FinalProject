#version 450

// Each light type have their own shader, chosen using preprocessor directive
// #define LIGHT_TYPE [POINT_LIGHT | DIRECTIONAL_LIGHT | SPOT_LIGHT]
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2

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
			fragColor = vec4(ambientColor + diffuseColor + specularColor, material.transparency);
		} else {
			vec3 R = reflect(-L, N);
			float specularStrength = pow(max(dot(V, R),0.0), material.shininess);
			vec3 specularColor = specularStrength * (lightIntensities.b * lightColor) * material.ks;
			fragColor = vec4(ambientColor + diffuseColor + specularColor, material.transparency);
		}
	}
}