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
	// start with ambient
	vec3 ambient = material.ambientK * light.color;

	// diffuse lighting
	// get a vector that points to the light direction
	vec3 newNormal = normalize(normal);
	vec3 omega = normalize(light.position - pos);
	
	// calculate the light intensity (I0 is just light.color)
	vec3 diffuse = material.diffuseK * (max(dot(newNormal, omega),0)) * light.color;

	// specular lighting
	vec3 reflection = reflect(-omega, normal);

	vec3 specular = material.specularK * pow(max(dot(reflection, camView), 0), material.shininess) * light.color;
	return (ambient + diffuse + specular);
}

void main(){

	vec3 result;
	vec3 camera = normalize(_CameraPosition -  fs_in.WorldPosition);
	for(int i = 0; i < _LightAmount; i++) {
		result += MakeLight(_Lights[i], _Material, camera, fs_in.WorldNormal, fs_in.WorldPosition);
	}

	vec3 OutColor = result * texture(_Texture,fs_in.UV).rgb;

	FragColor = vec4(OutColor, 1.0);
}