#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}fs_in;

uniform sampler2D _Texture;


struct Light{ 
	vec3 position;
	vec3 color;
	float strength;

	vec3 diffuse;
	vec3 specular;
	
	// light properties
	float constant;
	float linear;
	float quadratic;
};

#define MAX_TEXTURES 10

struct Material{
	float ambientK;
	float diffuseK;
	float specularK;
	float shininess;

	sampler2D texture_diffuse;
	sampler2D texture_specular;
};

#define MAX_LIGHTS 6
uniform Light _Lights[MAX_LIGHTS];

// light material uniforms

uniform Material _Material;


// camera uniforms
uniform vec3 _CameraPosition;

uniform int _LightAmount;


vec3 MakeLight(Light light, Material material, vec3 camView, vec3 normal, vec3 pos)
{
	// diffuse lighting
	// get a vector that points to the light direction
	vec3 newNormal = normalize(normal);

	vec3 lightDirection = light.position - pos;
	float dist = length(lightDirection);
	lightDirection = normalize(lightDirection);

	if (dist < 1) {
		dist = 1;
	}

	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	// calculate the light intensity (I0 is just light.color)
	float diffuseFloat = (max(dot(newNormal, lightDirection),0));

	vec3 diffuse = light.diffuse * diffuseFloat * vec3(texture(material.texture_diffuse, fs_in.UV));

	// specular lighting
	vec3 halfway = normalize(lightDirection + camView);

	float specularFloat = pow(max(dot(newNormal, halfway), 0), material.shininess);
	
	vec3 specular = light.specular * specularFloat * vec3(texture(material.texture_specular, fs_in.UV));

	vec3 ambient = 0.050 * vec3(texture(material.texture_diffuse, fs_in.UV));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 outColor = (ambient * light.color) + (diffuse * light.color * light.strength) + (specular * light.color * light.strength);
	return outColor;
}

void main(){

	vec3 result;
	vec3 camera = normalize(_CameraPosition -  fs_in.WorldPosition);
	for(int i = 0; i < _LightAmount; i++) {
		result += MakeLight(_Lights[i], _Material, camera, fs_in.WorldNormal, fs_in.WorldPosition);
	}

	vec4 colorTexture = texture(_Material.texture_diffuse,fs_in.UV);


	FragColor = vec4(result, 1.0) * colorTexture;
}