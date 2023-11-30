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
	float range;
};

struct Material{
	float ambientK;
	float diffuseK;
	float specularK;
	float shininess;
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
	// calculate the light intensity (I0 is just light.color)
	float diffuse = material.diffuseK * (max(dot(newNormal, lightDirection),0)) / (dist / light.range);

	// specular lighting
	vec3 halfway = normalize(lightDirection + camView);

	float specular = material.specularK * pow(max(dot(newNormal, halfway), 0), material.shininess);

	vec3 outColor = (diffuse * light.color * light.strength) + (specular * light.color * light.strength);
	return outColor;
}

void main(){

	vec3 result;
	vec3 camera = normalize(_CameraPosition -  fs_in.WorldPosition);
	for(int i = 0; i < _LightAmount; i++) {
		result += MakeLight(_Lights[i], _Material, camera, fs_in.WorldNormal, fs_in.WorldPosition);
	}

	vec3 OutColor = (result + _Material.ambientK) * texture(_Texture,fs_in.UV).rgb;

	FragColor = vec4(OutColor, 1.0);
}