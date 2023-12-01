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
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float shininess;
	float opacity;

	bool hasDiffuse;
	bool hasSpecular;
	bool hasBump;

	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;

};

#define MAX_LIGHTS 6
uniform Light _Lights[MAX_LIGHTS];

// material uniform
uniform Material _Material;

// camera uniforms
uniform vec3 _CameraPosition;

// current light count
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

	vec3 diffuseColor = material.diffuseColor;
	vec3 specularColor = material.specularColor;

	if (material.hasDiffuse) {
		diffuseColor *= vec3(texture(material.texture_diffuse1, fs_in.UV));
	} 
	/*
	if (material.hasSpecular) {
		specularColor *= vec3(texture(material.texture_specular1, fs_in.UV));
	}*/

	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	// calculate the light intensity (I0 is just light.color)
	float diffuseFloat = (max(dot(newNormal, lightDirection),0.0)) * light.strength;

	vec3 diffuse = diffuseFloat * diffuseColor;

	// specular lighting
	vec3 halfway = normalize(lightDirection + camView);

	float specularFloat = pow(max(dot(newNormal, halfway), 0.0), material.shininess) * light.strength;
	
	vec3 specular = specularFloat * specularColor;

	vec3 ambient = 0.15 * diffuseColor;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 outColor = light.color * ambient;
	outColor += light.color * diffuse;
	outColor += light.color * specular;

	return outColor;
}
// TODO: change material struct to include booleans for every texture type, so the textures can be-
// excluded from the calculations and only just use the diffuse/specular base colors.
// This should apply also to FragColor (instead of using a texture as the output use a base vec3 color like-
// usual.
void main(){

	vec3 result;
	vec3 camera = normalize(_CameraPosition -  fs_in.WorldPosition);
	for(int i = 0; i < _LightAmount; i++) {
		result += MakeLight(_Lights[i], _Material, camera, fs_in.WorldNormal, fs_in.WorldPosition);
	}

	vec4 color;


	if (_Material.hasDiffuse) {
		color = texture(_Material.texture_diffuse1,fs_in.UV);
	} else {
		color = vec4(_Material.diffuseColor, _Material.opacity);
	}

	FragColor = vec4(result, 1.0) * color;
}