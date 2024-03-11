#version 450

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout (std140) uniform Material 
{
	vec3 kd;
	vec3 ks;
	float shininess;
	float transparency;
} material;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform vec3 objectColor;

layout (location = 0) out vec4 fragColor;

void main()
{	
	float ambientStrength = 0.1;

	// Ambient 
	vec3 ambient = ambientStrength * lightColor;
	
	// Diffuse
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffuseComponent = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diffuseComponent * lightColor * material.kd;

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(cameraPos - fragPos);
	float specularComponent = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	vec3 specular = material.ks * specularComponent * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	fragColor = vec4(result, material.transparency);
}