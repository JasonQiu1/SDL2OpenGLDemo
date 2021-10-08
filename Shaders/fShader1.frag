#version 330 core
#define NUM_POINT_LIGHTS 1

out vec4 FragColor;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D emission;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform DirLight dirLight;
uniform PointLight pntLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;
uniform vec3 viewPos;
uniform float time;

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 diffuseColor, vec3 specColor, vec3 viewDir) {
	// ambient
	vec3 ambient = light.ambient * diffuseColor;

	// diffuse
	vec3 lightDir = -normalize(light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseColor;

	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * specColor;

	return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 diffuseColor, vec3 specColor, vec3 viewDir, vec3 fragPos) {
	// ambient
	vec3 ambient = light.ambient * diffuseColor;

	// diffuse
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseColor;

	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * specColor;

	// attenuation
	float dist = distance(light.position, fragPos);
	float attenuation = 1.0f / (light.linear + (light.linear * dist) + (light.quadratic * pow(dist, 2)));

	return attenuation * (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 diffuseColor, vec3 specColor, vec3 viewDir, vec3 fragPos) {
	// ambient
	vec3 ambient = light.ambient * diffuseColor;

	// check if in SpotLight
	vec3 lightDir = normalize(light.direction - fragPos);
	float theta = dot(normalize(-light.direction), lightDir);

	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3 (0.0f);
	if (theta > light.cutoff) {
		// diffuse
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse = light.diffuse * diff * diffuseColor;

		// specular
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * spec * specColor;
	}

	// attenuation
	float dist = distance(light.position, fragPos);
	float attenuation = 1.0f / (light.linear + (light.linear * dist) + (light.quadratic * pow(dist, 2)));

	return attenuation * (ambient + diffuse + specular);
}

void main() {
	// used by all lights
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 norm = normalize(Normal);
	vec3 diffuseColor = texture(material.texture_diffuse1, TexCoords).rgb;
	vec3 specColor = texture(material.texture_specular1, TexCoords).rgb;

	vec3 lightDir;
	vec3 reflectDir;

	// add together lights
	vec3 result = vec3(0.0f);
	result += CalcDirLight(dirLight, norm, diffuseColor, specColor, viewDir);
	result += CalcSpotLight(spotLight, norm, diffuseColor, specColor, viewDir, FragPos);
	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		result += CalcPointLight(pntLights[i], norm, diffuseColor, specColor, viewDir, FragPos);
	}

	// emission
	// vec3 emission = abs(sin(time)) * (texture(material.texture_specular1, TexCoords).rgb == vec3(0.0f) ? texture(material.emission, TexCoords).rgb : vec3(0.0f));

	// FragColor = vec4(result + emission, 1.0);
	FragColor = vec4(result, 1.0);
}